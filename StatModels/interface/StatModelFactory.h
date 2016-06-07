/*! Definition of the HH StatModel factory.
This file is part of https://github.com/cms-hh/StatAnalysis. */

#pragma once

#include <unordered_set>
#include "StatModel.h"

namespace hh_analysis {
namespace stat_models {

class StatModelFactory {
public:
    using ModelNameSet = std::unordered_set<std::string>;
    static StatModelPtr Make(const std::string& model_name);
    static const ModelNameSet& AvailableModelNames();
    static void ReportAvailableModelNames(std::ostream& os);

private:
    ~StatModelFactory() {}
};

} // namespace stat_models
} // namespace hh_analysis
