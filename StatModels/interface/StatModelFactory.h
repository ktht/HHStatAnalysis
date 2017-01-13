/*! Definition of the HH StatModel factory.
This file is part of https://github.com/cms-hh/HHStatAnalysis. */

#pragma once

#include <unordered_set>
#include "StatModel.h"
#include "StatModelDescriptor.h"

namespace hh_analysis {
namespace stat_models {

class StatModelFactory {
public:
    using ModelNameSet = std::unordered_set<std::string>;
    static StatModelPtr Make(const StatModelDescriptor& model_desc);
    static const ModelNameSet& AvailableModelNames();
    static void ReportAvailableModelNames(std::ostream& os);

private:
    ~StatModelFactory() {}
};

} // namespace stat_models
} // namespace hh_analysis
