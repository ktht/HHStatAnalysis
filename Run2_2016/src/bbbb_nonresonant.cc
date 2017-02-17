/*! Stat model for h->hh->bbbb.
This file is part of https://github.com/cms-hh/HHStatAnalysis. */

#include <iostream>
#include "CombineHarvester/CombineTools/interface/Systematics.h"
#include "CombineHarvester/CombineTools/interface/CardWriter.h"
#include "CombineHarvester/CombinePdfs/interface/MorphFunctions.h"
#include "HHStatAnalysis/Run2_2016/interface/bbbb_nonresonant.h"
#include "HHStatAnalysis/Core/interface/exception.h"
#include "HHStatAnalysis/Core/interface/Tools.h"
#include "HHStatAnalysis/Core/interface/TextIO.h"
#include "HHStatAnalysis/Core/interface/RootExt.h"
#include "HHStatAnalysis/Run2_2016/interface/CommonUncertainties.h"

namespace hh_analysis {
namespace stat_models {
namespace Run2_2016 {

const StatModel::v_str bbbb_nonresonant::ana_name = { "HHbbbb" };
const StatModel::v_str bbbb_nonresonant::eras = { "13TeV" };
const StatModel::v_str bbbb_nonresonant::bkg_processes = { "bkg_hem_mix" };

bbbb_nonresonant::bbbb_nonresonant(const StatModelDescriptor& _desc) :
    StatModel(_desc), signal_processes({ desc.signal_process })
{
}

std::string bbbb_nonresonant::NumToName(double x)
{
    std::ostringstream ss;
    ss << x;
    std::string str = ss.str();
    std::replace(str.begin(), str.end(), '-', 'm');
    std::replace(str.begin(), str.end(), '.', 'p');
    return str;
}

bbbb_nonresonant::ShapeNameRule bbbb_nonresonant::BackgroundShapeNameRule() const
{
    std::pair<std::string, std::string> result;
    std::ostringstream ss;
    ss << "$PROCESS";
    result.first = ss.str();
    ss << "_$SYSTEMATIC";
    result.second = ss.str();
    return result;
}

bbbb_nonresonant::ShapeNameRule bbbb_nonresonant::SignalShapeNameRule(double /*point_value*/) const
{
    std::pair<std::string, std::string> result;
    std::ostringstream ss;
//    ss << "$BIN/$PROCESS_" << desc.signal_point_prefix << "_" << NumToName(point_value);
    ss << "$PROCESS";
    result.first = ss.str();
    ss << "_$SYSTEMATIC";
    result.second = ss.str();
    return result;
}

void bbbb_nonresonant::ExtractShapes(ch::CombineHarvester& ch, const std::string& shapes_file, bool is_signal) const
{
    if(is_signal) {
        for(double x : desc.grid_x) {
            const auto& shape_name_rules = SignalShapeNameRule(x);
            ch.cp().process(signal_processes).mass({ToString(x)})
              .ExtractShapes(shapes_file, shape_name_rules.first, shape_name_rules.second);
        }
    } else {
        const auto& shape_name_rules = BackgroundShapeNameRule();
        ch.cp().process(bkg_processes).ExtractShapes(shapes_file, shape_name_rules.first, shape_name_rules.second);
    }
}

ch::Categories bbbb_nonresonant::GetChannelCategories(const std::string& channel)
{
    ch::Categories ch_categories;
    for(size_t n = 0; n < desc.categories.size(); ++n) {
        const std::string cat_name = desc.categories.at(n);
        const std::string name = boost::str(boost::format("%1%_%2%") % channel % cat_name);
        ch_categories.push_back({n, name});
    }
    return ch_categories;
}

void bbbb_nonresonant::CreateDatacards(const std::string& shapes_file, const std::string& output_path)
{
    static constexpr double br_H_bb = 5.809e-01;
    static constexpr double br_HH_bbbb = 2 * br_H_bb * br_H_bb;
    static constexpr double cr_HH = 33.41e-03;
    static constexpr double fb_to_pb = 1e3;
    static constexpr double sf = cr_HH * br_HH_bbbb * fb_to_pb;

    ch::CombineHarvester harvester;

    desc.signal_points.clear();
    for(double x : desc.grid_x)
        desc.signal_points.push_back(analysis::ToString(x));

    for(const auto& channel : desc.channels) {
        const auto& ch_categories = GetChannelCategories(channel);
        harvester.AddObservations(wildcard, ana_name, eras, {channel}, ch_categories);
        harvester.AddProcesses(desc.signal_points, ana_name, eras, {channel}, signal_processes, ch_categories, true);
        harvester.AddProcesses(wildcard, ana_name, eras, {channel}, bkg_processes, ch_categories, false);
    }

    AddSystematics(harvester);
    ExtractShapes(harvester, shapes_file, false);
    ExtractShapes(harvester, shapes_file, true);

    if(desc.limit_type == LimitType::SM) {
        harvester.cp().process(signal_processes).ForEachProc([](ch::Process *p) {
            p->set_rate(p->rate() * sf);
        });
    }

    FixNegativeBins(harvester);
    ch::SetStandardBinNames(harvester);

    std::string output_pattern = "/$TAG/$MASS/$BIN.txt";

    ch::CardWriter writer(output_path + output_pattern, output_path + "/$TAG/hh_bbbb_input.root");
    if(desc.combine_channels)
        writer.WriteCards("cmb", harvester);
    if(desc.per_channel_limits) {
        for(const auto& chn : desc.channels)
            writer.WriteCards(chn, harvester.cp().channel({chn}));
    }
}

void bbbb_nonresonant::AddSystematics(ch::CombineHarvester& cb)
{
    static constexpr size_t n_bins = 202;
    using CU = CommonUncertainties;

    CU::lumi().ApplyGlobal(cb, signal_processes);
    CU::scale_j().DistrType(UncDistributionType::shape).UseEra(false).Apply(cb, signal_processes);
    CU::res_j().UseEra(false).Apply(cb, signal_processes);

    const Uncertainty bkg_hem_mix_norm("bkg_hem_mix_norm", CorrelationRange::Analysis, UncDistributionType::lnU);
    bkg_hem_mix_norm.Apply(cb, 51.0, bkg_processes);

    const Uncertainty eff_b_jes("eff_b_jes", CorrelationRange::Experiment, UncDistributionType::shape);
    const Uncertainty eff_b_lf("eff_b_lf", CorrelationRange::Experiment, UncDistributionType::shape);
    const Uncertainty eff_b_hf("eff_b_hf", CorrelationRange::Experiment, UncDistributionType::shape);
    const Uncertainty eff_b_lfstats1("eff_b_lfstats1", CorrelationRange::Experiment, UncDistributionType::shape);
    const Uncertainty eff_b_lfstats2("eff_b_lfstats2", CorrelationRange::Experiment, UncDistributionType::shape);
    const Uncertainty eff_b_hfstats1("eff_b_hfstats1", CorrelationRange::Experiment, UncDistributionType::shape);
    const Uncertainty eff_b_hfstats2("eff_b_hfstats2", CorrelationRange::Experiment, UncDistributionType::shape);
    const Uncertainty eff_b_cferr1("eff_b_cferr1", CorrelationRange::Experiment, UncDistributionType::shape);
    const Uncertainty eff_b_cferr2("eff_b_cferr2", CorrelationRange::Experiment, UncDistributionType::shape);

    eff_b_jes.UseEra(false).Apply(cb, signal_processes);
    eff_b_lf.UseEra(false).Apply(cb, signal_processes);
    eff_b_hf.UseEra(false).Apply(cb, signal_processes);
    eff_b_lfstats1.UseEra(false).Apply(cb, signal_processes);
    eff_b_lfstats2.UseEra(false).Apply(cb, signal_processes);
    eff_b_hfstats1.UseEra(false).Apply(cb, signal_processes);
    eff_b_hfstats2.UseEra(false).Apply(cb, signal_processes);
    eff_b_cferr1.UseEra(false).Apply(cb, signal_processes);
    eff_b_cferr2.UseEra(false).Apply(cb, signal_processes);

    for(size_t bin_id = 1; bin_id < n_bins; ++bin_id) {
        const Uncertainty bin_unc("", CorrelationRange::Analysis, UncDistributionType::shape);
        bin_unc.UseEra(false).ApplyBinByBin(cb, bkg_processes.at(0), bin_id);
    }

    if(desc.limit_type != LimitType::SM) return;
    CU::cr_SM_HH().ApplyGlobal(cb, signal_processes);
    const double br_SM_H_bb_unc = 1. + 2. * (CU::br_SM_H_bb().value - 1.);
    CU::br_SM_H_bb().Apply(cb, br_SM_H_bb_unc, signal_processes);
}

} // namespace Run2_2016
} // namespace stat_models
} // namespace hh_analysis
