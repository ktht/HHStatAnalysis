#pragma once

#include "hh-limits/Run2Models/interface/HH_Model.h"

namespace hh_limits {
namespace unc_models {

class bbtautau_Resonant : public HH_Model {
public:
    static const std::string file_name_suffix;
    static const v_double masses;
    static const v_str masses_str;
    static const v_str ana_name;
    static const v_str eras;
    static const v_str channels;
    static const ch::Categories categories;
    static const v_str signal_processes;
    static const v_str bkg_mc_processes;
    static const v_str bkg_data_driven_processes;
    static const v_str bkg_all_processes;
    static const v_str all_mc_processes;

    static constexpr double bbb_unc_threshold = 0.1;
    static constexpr double bin_merge_threashold = 0.5;

    virtual void CreateDatacards(const std::string& shapes_path, const std::string& output_path) override;

private:
    void AddSystematics(ch::CombineHarvester& combine_harvester);
    static std::pair<std::string, std::string> ShapeNameRule(bool use_mass);
    static void ExtractShapes(ch::CombineHarvester& combine_harvester, const std::string& path, bool is_signal);
    static ch::Categories GetChannelCategories(const std::string& channel);
    static std::string FullFileName(const std::string& path, const std::string& channel);
};

} // namespace models
} // namespace limits
