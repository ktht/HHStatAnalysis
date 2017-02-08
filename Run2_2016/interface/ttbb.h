/*! Common part of stat models for the hh->bbtautau analyses.
This file is part of https://github.com/cms-hh/HHStatAnalysis. */

#pragma once

#include "HHStatAnalysis/StatModels/interface/StatModel.h"

namespace hh_analysis {
namespace stat_models {
namespace Run2_2016 {

class ttbb_base : public StatModel {
public:
    static const v_str ana_name;
    static const v_str eras;
    static const v_str bkg_mc_processes;
    static const v_str bkg_data_driven_processes;
    static const v_str bkg_all_processes;

    static constexpr double bbb_unc_threshold = 0.1;
    static constexpr double bin_merge_threashold = 0.5;

    ttbb_base(const StatModelDescriptor& _desc);

protected:
    virtual void AddSystematics(ch::CombineHarvester& combine_harvester);
    ch::Categories GetChannelCategories(const std::string& channel);

protected:
    const v_str signal_processes, all_mc_processes;
};

} // namespace Run2_2016
} // namespace stat_models
} // namespace hh_analysis
