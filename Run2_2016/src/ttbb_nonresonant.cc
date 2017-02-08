/*! Stat model for h->hh->bbtautau.
This file is part of https://github.com/cms-hh/HHStatAnalysis. */

#include <iostream>
#include "CombineHarvester/CombineTools/interface/Systematics.h"
#include "CombineHarvester/CombineTools/interface/CardWriter.h"
#include "CombineHarvester/CombinePdfs/interface/MorphFunctions.h"
#include "HHStatAnalysis/Run2_2016/interface/ttbb_nonresonant.h"
#include "HHStatAnalysis/Core/interface/exception.h"
#include "HHStatAnalysis/Core/interface/Tools.h"
#include "HHStatAnalysis/Core/interface/TextIO.h"
#include "HHStatAnalysis/Core/interface/RootExt.h"
#include "HHStatAnalysis/Run2_2016/interface/CommonUncertainties.h"

namespace hh_analysis {
namespace stat_models {
namespace Run2_2016 {

std::string ttbb_nonresonant::NumToName(double x)
{
    std::ostringstream ss;
    ss << x;
    std::string str = ss.str();
    std::replace(str.begin(), str.end(), '-', 'm');
    std::replace(str.begin(), str.end(), '.', 'p');
    return str;
}

ttbb_nonresonant::ShapeNameRule ttbb_nonresonant::BackgroundShapeNameRule() const
{
    std::pair<std::string, std::string> result;
    std::ostringstream ss;
    ss << "$BIN/$PROCESS";
    result.first = ss.str();
    ss << "_$SYSTEMATIC";
    result.second = ss.str();
    return result;
}

ttbb_nonresonant::ShapeNameRule ttbb_nonresonant::SignalShapeNameRule(double point_value) const
{
    std::pair<std::string, std::string> result;
    std::ostringstream ss;
    ss << "$BIN/$PROCESS_" << desc.signal_point_prefix << "_" << NumToName(point_value);
    result.first = ss.str();
    ss << "_$SYSTEMATIC";
    result.second = ss.str();
    return result;
}

void ttbb_nonresonant::ExtractShapes(ch::CombineHarvester& ch, const std::string& shapes_file,
                                      bool is_signal) const
{
    if(is_signal) {
        for(double x : desc.grid_x) {
            const auto& shape_name_rules = SignalShapeNameRule(x);
            ch.cp().process(signal_processes).mass({ToString(x)})
              .ExtractShapes(shapes_file, shape_name_rules.first, shape_name_rules.second);
        }
    } else {
        const auto& shape_name_rules = BackgroundShapeNameRule();
        ch.cp().process(bkg_all_processes).ExtractShapes(shapes_file, shape_name_rules.first, shape_name_rules.second);
    }
}

void ttbb_nonresonant::CreateDatacards(const std::string& shapes_file, const std::string& output_path)
{
    static constexpr double br_H_tautau = 6.256e-02;
    static constexpr double br_H_bb = 5.809e-01;
    static constexpr double br_HH_bbtautau = 2 * br_H_tautau * br_H_bb;
    static constexpr double cr_HH = 33.41e-03;
    static constexpr double sf = cr_HH * br_HH_bbtautau;

    ch::CombineHarvester harvester;

    desc.signal_points.clear();
    for(double x : desc.grid_x)
        desc.signal_points.push_back(analysis::ToString(x));

    for(const auto& channel : desc.channels) {
        const auto& ch_categories = GetChannelCategories(channel);
        harvester.AddObservations(wildcard, ana_name, eras, {channel}, ch_categories);
        harvester.AddProcesses(desc.signal_points, ana_name, eras, {channel}, signal_processes, ch_categories, true);
        harvester.AddProcesses(wildcard, ana_name, eras, {channel}, bkg_all_processes, ch_categories, false);
    }

    AddSystematics(harvester);
    ExtractShapes(harvester, shapes_file, false);
    ExtractShapes(harvester, shapes_file, true);

    if(desc.limit_type == LimitType::SM) {
        harvester.cp().process(signal_processes).ForEachProc([](ch::Process *p) {
            p->set_rate(p->rate() * sf);
        });
    }

    if(desc.model_signal_process.size())
        RenameProcess(harvester, desc.signal_process, desc.model_signal_process);

    FixNegativeBins(harvester);
    harvester.cp().backgrounds().MergeBinErrors(bbb_unc_threshold, bin_merge_threashold);
    harvester.cp().backgrounds().AddBinByBin(bbb_unc_threshold, true, &harvester);
    ch::SetStandardBinNames(harvester);

    std::string output_pattern = "/$TAG/$MASS/$BIN.txt";

    ch::CardWriter writer(output_path + output_pattern, output_path + "/$TAG/hh_ttbb_input.root");
    if(desc.morph)
        writer.SetWildcardMasses({});
    if(desc.combine_channels)
        writer.WriteCards("cmb", harvester);
    if(desc.per_channel_limits) {
        for(const auto& chn : desc.channels)
            writer.WriteCards(chn, harvester.cp().channel({chn}));
    }
}

void ttbb_nonresonant::AddSystematics(ch::CombineHarvester& cb)
{
    using CU = CommonUncertainties;
    ttbb_base::AddSystematics(cb);
    if(desc.limit_type != LimitType::SM) return;

    CU::cr_SM_HH().ApplyGlobal(cb, signal_processes);
    CU::br_SM_H_tautau().ApplyGlobal(cb, signal_processes);
    CU::br_SM_H_bb().ApplyGlobal(cb, signal_processes);
}

} // namespace Run2_2016
} // namespace stat_models
} // namespace hh_analysis
