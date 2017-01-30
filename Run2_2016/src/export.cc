/*! Shared library exports.
This file is part of https://github.com/cms-hh/StatAnalysis. */

#include "HHStatAnalysis/Run2_2016/interface/ttbb_resonant.h"

extern "C" hh_analysis::stat_models::StatModelPtr create_stat_model(
        const char* stat_model_name, const hh_analysis::StatModelDescriptor* model_descriptor)
{
    using namespace hh_analysis::stat_models::Run2_2016;

    if(!stat_model_name || !model_descriptor)
        throw analysis::exception("Null pointer is passed to create_stat_model function.");

    const std::string name(stat_model_name);
    if(name == "ttbb_resonant")
        return std::make_shared<ttbb_resonant>(*model_descriptor);
    throw analysis::exception("Stat model '%1%' not found.") % name;
}
