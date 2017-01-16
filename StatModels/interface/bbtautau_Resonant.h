/*! Stat model for X->hh->bbtautau.
This file is part of https://github.com/cms-hh/HHStatAnalysis. */

#pragma once

#include "StatModel.h"

namespace hh_analysis {
namespace stat_models {

class bbtautau_Resonant : public StatModel {
public:
    static const v_str ana_name;
    static const v_str eras;
    static const v_str bkg_mc_processes;
    static const v_str bkg_data_driven_processes;
    static const v_str bkg_all_processes;

    static constexpr double bbb_unc_threshold = 0.1;
    static constexpr double bin_merge_threashold = 0.5;

    bbtautau_Resonant(const StatModelDescriptor& _desc);
    virtual void CreateDatacards(const std::string& shapes_file, const std::string& output_path) override;

private:
    void AddSystematics(ch::CombineHarvester& combine_harvester);
    std::pair<std::string, std::string> ShapeNameRule(bool use_mass) const;
    void ExtractShapes(ch::CombineHarvester& combine_harvester, const std::string& shapes_file, bool is_signal) const;
    ch::Categories GetChannelCategories(const std::string& channel);
    std::string FullFileName(const std::string& path, const std::string& channel) const;

private:
    const v_str signal_processes, all_mc_processes;
};

} // namespace stat_models
} // namespace hh_analysis
