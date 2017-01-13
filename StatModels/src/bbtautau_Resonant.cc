/*! Stat model for X->hh->bbtautau.
This file is part of https://github.com/cms-hh/HHStatAnalysis. */

#include <iostream>
#include "CombineHarvester/CombineTools/interface/Systematics.h"
#include "CombineHarvester/CombineTools/interface/CardWriter.h"
#include "CombineHarvester/CombinePdfs/interface/MorphFunctions.h"
#include "HHStatAnalysis/StatModels/interface/bbtautau_Resonant.h"
#include "HHStatAnalysis/Core/interface/exception.h"
#include "HHStatAnalysis/Core/interface/Tools.h"
#include "HHStatAnalysis/Core/interface/TextIO.h"
#include "HHStatAnalysis/Core/interface/RootExt.h"

namespace hh_analysis {
namespace stat_models {

const StatModel::v_str bbtautau_Resonant::ana_name = { "hh_ttbb" };
const StatModel::v_str bbtautau_Resonant::eras = { "13TeV" };
const StatModel::v_str bbtautau_Resonant::bkg_mc_processes = { "DY_0b", "DY_1b", "DY_2b", "TT", "tW", "VV", "W" };
const StatModel::v_str bbtautau_Resonant::bkg_data_driven_processes = { "QCD" };
const StatModel::v_str bbtautau_Resonant::bkg_all_processes
                                        = ch::JoinStr({ bkg_mc_processes, bkg_data_driven_processes });

bbtautau_Resonant::bbtautau_Resonant(const StatModelDescriptor& _desc) :
    StatModel(_desc), signal_processes({ desc.signal_process }),
    all_mc_processes(ch::JoinStr({ signal_processes, bkg_mc_processes }))
{
}

std::pair<std::string, std::string> bbtautau_Resonant::ShapeNameRule(bool use_mass) const
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

void bbtautau_Resonant::ExtractShapes(ch::CombineHarvester& combine_harvester, const std::string& shapes_file,
                                      bool is_signal) const
{
    const auto& processes = is_signal ? signal_processes : bkg_all_processes;
    const auto& shape_name_rules = ShapeNameRule(is_signal);
    combine_harvester.cp().process(processes).ExtractShapes(shapes_file, shape_name_rules.first,
                                                            shape_name_rules.second);
}

ch::Categories bbtautau_Resonant::GetChannelCategories(const std::string& channel)
{
    ch::Categories ch_categories;
    for(size_t n = 0; n < desc.categories.size(); ++n) {
        const std::string cat_name = desc.categories.at(n);
        const std::string name = boost::str(boost::format("%1%_%2%") % channel % cat_name);
        ch_categories.push_back({n, name});
    }
    return ch_categories;
}

void bbtautau_Resonant::CreateDatacards(const std::string& shapes_file, const std::string& output_path)
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

void bbtautau_Resonant::AddSystematics(ch::CombineHarvester& cb)
{
    using ch::syst::SystMap;
    ch::CombineHarvester h = cb.cp();

    h.cp().process(all_mc_processes).AddSyst(cb, "lumi_13TeV", "lnN", SystMap<>::init(1.027));
    h.cp().process(signal_processes).AddSyst(cb, "scale_j_13TeV", "lnN", SystMap<>::init(1.02));
    h.cp().process(bkg_mc_processes).AddSyst(cb, "scale_j_13TeV", "lnN", SystMap<>::init(1.04));
    h.cp().process({"TT"}).AddSyst(cb, "TT_xs_13TeV", "lnN", SystMap<>::init(1.05));
    h.cp().process(all_mc_processes).AddSyst(cb, "scale_b", "lnN", SystMap<>::init(1.02));
    h.cp().process(all_mc_processes).AddSyst(cb, "eff_btag", "lnN", SystMap<>::init(1.03));
    h.cp().process(all_mc_processes).AddSyst(cb, "eff_t", "lnN", SystMap<>::init(1.06));
    h.cp().process(all_mc_processes).AddSyst(cb, "eff_m", "lnN", SystMap<>::init(1.018));
    h.cp().process({"QCD"}).AddSyst(cb, "qcd_norm", "lnN", SystMap<>::init(1.06));
}

} // namespace stat_models
} // namespace hh_analysis
