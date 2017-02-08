/*! Stat model for h->hh->bbtautau.
This file is part of https://github.com/cms-hh/HHStatAnalysis. */

#pragma once

#include "ttbb.h"

namespace hh_analysis {
namespace stat_models {
namespace Run2_2016 {

class ttbb_nonresonant : public ttbb_base {
public:
    using ttbb_base::ttbb_base;
    virtual void CreateDatacards(const std::string& shapes_file, const std::string& output_path) override;

protected:
    virtual void AddSystematics(ch::CombineHarvester& combine_harvester) override;

private:
    using ShapeNameRule = std::pair<std::string, std::string>;
    static std::string NumToName(double x);
    ShapeNameRule BackgroundShapeNameRule() const;
    ShapeNameRule SignalShapeNameRule(double point_value) const;
    void ExtractShapes(ch::CombineHarvester& combine_harvester, const std::string& shapes_file, bool is_signal) const;
};

} // namespace Run2_2016
} // namespace stat_models
} // namespace hh_analysis
