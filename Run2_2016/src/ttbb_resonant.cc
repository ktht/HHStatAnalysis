/*! Stat model for X->hh->bbtautau.
This file is part of https://github.com/cms-hh/HHStatAnalysis. */

#include <iostream>
#include "CombineHarvester/CombineTools/interface/Systematics.h"
#include "CombineHarvester/CombineTools/interface/CardWriter.h"
#include "CombineHarvester/CombinePdfs/interface/MorphFunctions.h"
#include "HHStatAnalysis/Run2_2016/interface/ttbb_resonant.h"
#include "HHStatAnalysis/Core/interface/exception.h"
#include "HHStatAnalysis/Core/interface/Tools.h"
#include "HHStatAnalysis/Core/interface/TextIO.h"
#include "HHStatAnalysis/Core/interface/RootExt.h"
#include "HHStatAnalysis/Run2_2016/interface/CommonUncertainties.h"

namespace hh_analysis {
namespace stat_models {
namespace Run2_2016 {

std::pair<std::string, std::string> ttbb_resonant::ShapeNameRule(bool use_mass) const
{
    std::pair<std::string, std::string> result;
    std::ostringstream ss;
    ss << "$BIN/$PROCESS";
    if(use_mass)
        ss << "_" << desc.signal_point_prefix << "$MASS";
    result.first = ss.str();
    ss << "_$SYSTEMATIC";
    result.second = ss.str();
    return result;
}

void ttbb_resonant::ExtractShapes(ch::CombineHarvester& combine_harvester, const std::string& shapes_file,
                                      bool is_signal) const
{
    const auto& processes = is_signal ? signal_processes : bkg_all_processes;
    const auto& shape_name_rules = ShapeNameRule(is_signal);
    combine_harvester.cp().process(processes).ExtractShapes(shapes_file, shape_name_rules.first,
                                                            shape_name_rules.second);
}

void ttbb_resonant::CreateDatacards(const std::string& shapes_file, const std::string& output_path)
{
    ch::CombineHarvester harvester;

    for(const auto& channel : desc.channels) {
        const auto& ch_categories = GetChannelCategories(channel);
        harvester.AddObservations(wildcard, ana_name, eras, {channel}, ch_categories);
        harvester.AddProcesses(desc.signal_points, ana_name, eras, {channel}, signal_processes, ch_categories, true);
        harvester.AddProcesses(wildcard, ana_name, eras, {channel}, bkg_all_processes, ch_categories, false);
    }

    AddSystematics(harvester);
    ExtractShapes(harvester, shapes_file, false);
    ExtractShapes(harvester, shapes_file, true);
    if(desc.model_signal_process.size())
        RenameProcess(harvester, desc.signal_process, desc.model_signal_process);

    FixNegativeBins(harvester);    
    harvester.cp().backgrounds().MergeBinErrors(bbb_unc_threshold, bin_merge_threashold);
    harvester.cp().backgrounds().AddBinByBin(bbb_unc_threshold, true, &harvester);
    ch::SetStandardBinNames(harvester);

    std::shared_ptr<RooWorkspace> workspace;
    RooRealVar mH("mH", "mH", Parse<double>(desc.signal_points.front()), Parse<double>(desc.signal_points.back()));
    std::string output_pattern = "/$TAG/$MASS/$BIN.txt";
    if(desc.morph) {
        workspace = std::make_shared<RooWorkspace>("hh_ttbb", "hh_ttbb");
        for(const auto& bin : harvester.bin_set())
            ch::BuildRooMorphing(*workspace, harvester, bin, desc.model_signal_process, mH, "norm", true, true, true);
        harvester.AddWorkspace(*workspace);
        harvester.cp().process({desc.model_signal_process}).ExtractPdfs(harvester, workspace->GetName(),
                                                                        "$BIN_$PROCESS_morph");
        output_pattern = "/$TAG/$BIN.txt";
    }

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

} // namespace Run2_2016
} // namespace stat_models
} // namespace hh_analysis
