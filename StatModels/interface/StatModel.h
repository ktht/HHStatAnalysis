/*! Definition of the base class for HH stat models.
This file is part of https://github.com/cms-hh/StatAnalysis. */

#pragma once

#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"

namespace hh_analysis {
namespace stat_models {

class StatModel {
public:
    using v_str = std::vector<std::string>;
    using v_double = std::vector<double>;

    static const v_str wildcard;

    virtual ~StatModel() {}
    virtual void CreateDatacards(const std::string& shapes_path, const std::string& output_path) = 0;
};

using StatModelPtr = std::shared_ptr<StatModel>;

} // namespace stat_models
} // namespace hh_analysis
