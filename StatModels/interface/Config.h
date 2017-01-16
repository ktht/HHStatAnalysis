/*! Functions to read stat model configuration.
This file is part of https://github.com/cms-hh/HHStatAnalysis. */

#pragma once

#include "HHStatAnalysis/Core/interface/ConfigReader.h"
#include "HHStatAnalysis/StatModels/interface/ModelConfigEntryReader.h"

namespace hh_analysis {

inline void ReadConfig(const std::string& cfg_name, ModelDescriptorCollection& descs)
{
    analysis::ConfigReader config_reader;

    ModelConfigEntryReader reader(descs);
    config_reader.AddEntryReader("MODEL", reader, true);

    config_reader.ReadConfig(cfg_name);
}

inline StatModelDescriptor LoadDescriptor(const std::string& cfg_name, const std::string& model_desc)
{
    ModelDescriptorCollection descs;
    ReadConfig(cfg_name, descs);
    if(!descs.count(model_desc))
        throw analysis::exception("Unable to find %1% in %2%.") % model_desc % cfg_name;
    return descs.at(model_desc);
}

} // namespace hh_analysis
