/*! Stat model for X->hh->bbtautau.
This file is part of https://github.com/cms-hh/StatAnalysis. */

#include <iostream>
#include "CombineHarvester/CombineTools/interface/Systematics.h"
#include "HHStatAnalysis/StatModels/interface/bbtautau_Resonant.h"
#include "HHStatAnalysis/Core/interface/exception.h"
#include "HHStatAnalysis/Core/interface/Tools.h"

namespace hh_analysis {
namespace stat_models {

const std::string bbtautau_Resonant::file_name_suffix = "m_ttbb_MassWindow";
const StatModel::v_double bbtautau_Resonant::masses = {
    250, 260, 270, 280, 300, 320, 340, 350, 400, 450, 500, 550, 600, 650, 700, 800, 900
};

const StatModel::v_str bbtautau_Resonant::masses_str = ToStrVector(bbtautau_Resonant::masses);
const StatModel::v_str bbtautau_Resonant::ana_name = { "HHbbtt" };
const StatModel::v_str bbtautau_Resonant::eras = { "13TeV" };
const StatModel::v_str bbtautau_Resonant::channels = { "muTau" };
const ch::Categories bbtautau_Resonant::categories = {
    { 0, "2jet0tag" }, { 1, "2jet1tag" }, { 2, "2jet2tag" }
};
const StatModel::v_str bbtautau_Resonant::signal_processes = { "ggRadionTohhTo2Tau2B" };
const StatModel::v_str bbtautau_Resonant::bkg_mc_processes = { "TT", "ZTT", "VV", "W" };
const StatModel::v_str bbtautau_Resonant::bkg_data_driven_processes = { "QCD" };
const StatModel::v_str bbtautau_Resonant::bkg_all_processes
                                        = ch::JoinStr({ bkg_mc_processes, bkg_data_driven_processes });
const StatModel::v_str bbtautau_Resonant::all_mc_processes = ch::JoinStr({ signal_processes, bkg_mc_processes });

std::pair<std::string, std::string> bbtautau_Resonant::ShapeNameRule(bool use_mass)
{

    std::pair<std::string, std::string> result;
    std::ostringstream ss;
    ss << "$BIN/$PROCESS";
    if(use_mass)
        ss << "$MASS";
    result.first = ss.str();
    ss << "_$SYSTEMATIC";
    result.second = ss.str();
    return result;
}

void bbtautau_Resonant::ExtractShapes(ch::CombineHarvester& combine_harvester, const std::string& path, bool is_signal)
{
    const auto& processes = is_signal ? signal_processes : bkg_all_processes;
    const auto& shape_name_rules = ShapeNameRule(is_signal);
    for(const auto& channel : channels) {
        const auto& file_name = FullFileName(path, channel);
        combine_harvester.cp().channel({channel}).process(processes)
                .ExtractShapes(file_name, shape_name_rules.first, shape_name_rules.second);
    }
}

ch::Categories bbtautau_Resonant::GetChannelCategories(const std::string& channel)
{
    ch::Categories ch_categories;
    for(const auto& category : categories) {
        const std::string name = boost::str(boost::format("%1%_%2%") % channel % category.second);
        ch_categories.push_back({category.first, name});
    }
    return ch_categories;
}

std::string bbtautau_Resonant::FullFileName(const std::string& path, const std::string& channel)
{
    const std::string file_name = boost::str(boost::format("%1%_%2%.root") % channel % file_name_suffix);
    return FullPath({path, file_name});
}

void bbtautau_Resonant::CreateDatacards(const std::string& shapes_path, const std::string& output_path)
{
    ch::CombineHarvester harvester;

    for(const auto& channel : channels) {
        const auto& ch_categories = GetChannelCategories(channel);
        harvester.AddObservations(wildcard, ana_name, eras, {channel}, ch_categories);
        harvester.AddProcesses(masses_str, ana_name, eras, {channel}, signal_processes, ch_categories, true);
        harvester.AddProcesses(wildcard, ana_name, eras, {channel}, bkg_all_processes, ch_categories, false);
    }

    AddSystematics(harvester);
    ExtractShapes(harvester, shapes_path, false);
    ExtractShapes(harvester, shapes_path, true);

    harvester.cp().backgrounds().MergeBinErrors(bbb_unc_threshold, bin_merge_threashold);
    harvester.cp().backgrounds().AddBinByBin(bbb_unc_threshold, true, &harvester);

    for(const auto& channel : channels) {
        const std::string channel_path = FullPath({output_path, channel});
        boost::filesystem::create_directories(channel_path);

        const std::string root_file_name = boost::str(boost::format("hh_bbtt_%1%.input.root") % channel);

        const std::string root_file_full_name = FullPath({channel_path, root_file_name});
        auto root_file = CreateRootFile(root_file_full_name);

        for(const auto& mass : masses_str) {
            const std::string mass_path = FullPath({channel_path, mass});
            boost::filesystem::create_directories(mass_path);
            const std::string root_file_link_name = FullPath({mass_path, root_file_name});
            boost::filesystem::create_symlink(root_file_full_name, root_file_link_name);

            const v_str mass_pattern = { mass, wildcard.front() };
            for(const auto& category : harvester.cp().channel({channel}).bin_set()) {
                const std::string datacard_name = boost::str(boost::format("%1%.txt") % category);
                const std::string datacard_path = FullPath({mass_path, datacard_name});
                harvester.cp().channel({channel}).mass(mass_pattern).bin({category})
                        .WriteDatacard(datacard_path, *root_file);
            }
        }
    }
}

void bbtautau_Resonant::AddSystematics(ch::CombineHarvester& cb)
{
    using ch::syst::SystMap;
    ch::CombineHarvester h = cb.cp();
    const auto& sig = ch::Set2Vec(cb.cp().signals().process_set());
    const std::vector<std::string> bkg_mc = {"TT", "ZTT", "VV", "W"};
    const std::vector<std::string> mc_samples = ch::JoinStr({ sig, {"TT", "ZTT", "VV", "W"}});

    h.cp().process(mc_samples).AddSyst(cb, "lumi_13TeV", "lnN", SystMap<>::init(1.027));
    h.cp().process(sig).AddSyst(cb, "scale_j_13TeV", "lnN", SystMap<>::init(1.02));
    h.cp().process(bkg_mc).AddSyst(cb, "scale_j_13TeV", "lnN", SystMap<>::init(1.04));
    h.cp().process({"TT"}).AddSyst(cb, "TT_xs_13TeV", "lnN", SystMap<>::init(1.05));
    h.cp().process(mc_samples).AddSyst(cb, "CMS_scale_t_mutau_13TeV", "shape", SystMap<>::init(1));
}

} // namespace stat_models
} // namespace hh_analysis
