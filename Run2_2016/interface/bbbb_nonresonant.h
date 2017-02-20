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

    bbbb_nonresonant(const StatModelDescriptor& _desc, const std::string& input_file_name);
    virtual void CreateDatacards(const std::string& output_path) override;

private:
    virtual const v_str& SignalProcesses() const override { return signal_processes; }
    virtual const v_str& BackgroundProcesses() const override { return bkg_processes; }

    virtual ShapeNameRule SignalShapeNameRule() const override { return "$PROCESS"; }
    virtual ShapeNameRule BackgroundShapeNameRule() const override { return "$PROCESS"; }

    void AddSystematics(ch::CombineHarvester& combine_harvester);

private:
    const v_str signal_processes;
};

} // namespace Run2_2016
} // namespace stat_models
} // namespace hh_analysis
