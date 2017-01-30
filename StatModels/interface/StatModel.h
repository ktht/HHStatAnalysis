/*! Definition of the base class for HH stat models.
This file is part of https://github.com/cms-hh/HHStatAnalysis. */

#pragma once

#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"
#include "StatModelDescriptor.h"

namespace hh_analysis {
namespace stat_models {

class StatModel {
public:
    using v_str = std::vector<std::string>;
    using v_double = std::vector<double>;

    static const v_str wildcard;

    StatModel(const StatModelDescriptor& _desc) : desc(_desc) {}

    virtual ~StatModel() {}
    virtual void CreateDatacards(const std::string& shapes_file, const std::string& output_path) = 0;

    static void FixNegativeBins(ch::CombineHarvester& harvester);
    static void RenameProcess(ch::CombineHarvester& harvester, const std::string& old_name,
                              const std::string& new_name);

protected:
    StatModelDescriptor desc;
};

using StatModelPtr = std::shared_ptr<StatModel>;
using StatModelCreator = StatModelPtr (*)(const char*, const StatModelDescriptor*);

} // namespace stat_models
} // namespace hh_analysis
