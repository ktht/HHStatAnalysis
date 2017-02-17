/*! Stat model for h->hh->bbbb.
This file is part of https://github.com/cms-hh/HHStatAnalysis. */

#pragma once

#include "HHStatAnalysis/StatModels/interface/StatModel.h"

namespace hh_analysis {
namespace stat_models {
namespace Run2_2016 {

class bbbb_nonresonant : public StatModel {
public:
    static const v_str ana_name;
    static const v_str eras;
    static const v_str bkg_processes;

    bbbb_nonresonant(const StatModelDescriptor& _desc);
    virtual void CreateDatacards(const std::string& shapes_file, const std::string& output_path) override;

private:
    using ShapeNameRule = std::pair<std::string, std::string>;
    static std::string NumToName(double x);
    ShapeNameRule BackgroundShapeNameRule() const;
    ShapeNameRule SignalShapeNameRule(double point_value) const;
    void ExtractShapes(ch::CombineHarvester& combine_harvester, const std::string& shapes_file, bool is_signal) const;
    ch::Categories GetChannelCategories(const std::string& channel);

    void AddSystematics(ch::CombineHarvester& combine_harvester);

private:
    const v_str signal_processes;
};

} // namespace Run2_2016
} // namespace stat_models
} // namespace hh_analysis
