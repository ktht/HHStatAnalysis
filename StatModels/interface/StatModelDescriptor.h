/*! Definition of the stat model descriptor.
This file is part of https://github.com/cms-hh/HHStatAnalysis. */

#pragma once

#include <map>
#include "HHStatAnalysis/Core/interface/EnumNameMap.h"
#include "HHStatAnalysis/Core/interface/TextIO.h"
#include "HHStatAnalysis/Core/interface/NumericPrimitives.h"

namespace hh_analysis {
using namespace analysis;

enum class LimitType { ModelIndependent, SM, MSSM, NonResonant_BSM};
ENUM_NAMES(LimitType) = {
    { LimitType::ModelIndependent, "model_independent" },
    { LimitType::SM, "SM" },
    { LimitType::MSSM, "MSSM" },
    { LimitType::NonResonant_BSM, "NonResonant_BSM" }
};

struct StatModelDescriptor {
    std::string name;
    std::string stat_model;
    std::vector<std::string> channels;
    std::vector<std::string> categories;
    std::string signal_process, model_signal_process;
    std::string signal_point_prefix;
    std::vector<std::string> signal_points;

    LimitType limit_type;
    std::string th_model_file;
    bool blind, morph, combine_channels, per_channel_limits, per_category_limits;
    RangeWithStep<double> grid_x, grid_y;


    std::map<std::string, std::string> custom_params;

    template<typename T = std::string>
    T at(const std::string& key) const
    {
        if(!custom_params.count(key))
            throw exception("Custom parameter '%1%' not found.") % key;
        T value;
        const std::string& str_value = custom_params.at(key);
        if(!TryParse(str_value, value))
            throw exception("Unable to parse a value ('%1%') of the custom parameter '%2%'. ") % str_value % key;
        return value;
    }

    StatModelDescriptor() :
        limit_type(LimitType::ModelIndependent), blind(true), morph(false), combine_channels(true),
        per_channel_limits(false), per_category_limits(false) {}
};

using ModelDescriptorCollection = std::unordered_map<std::string, StatModelDescriptor>;

} // namespace hh_analysis
