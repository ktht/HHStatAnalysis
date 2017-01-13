/*! Implementation of the base class for HH stat models.
This file is part of https://github.com/cms-hh/HHStatAnalysis. */

#include "HHStatAnalysis/StatModels/interface/StatModel.h"

namespace hh_analysis {
namespace stat_models {

const StatModel::v_str StatModel::wildcard = { "*" };

void StatModel::FixNegativeBins(ch::CombineHarvester& harvester)
{
    harvester.ForEachProc([](ch::Process *p) {
        if(!ch::HasNegativeBins(p->shape())) return;
        std::cout << "[Negative bins] Fixing negative bins for " << p->bin() << "," << p->process() << "\n";
        auto new_shape = p->ClonedShape();
        ch::ZeroNegativeBins(new_shape.get());
        p->set_shape(std::move(new_shape), false);
    });
}

void StatModel::RenameProcess(ch::CombineHarvester& harvester, const std::string& old_name, const std::string& new_name)
{
    harvester.ForEachObj([&](ch::Object *obj) {
        if(obj->process() == old_name)
            obj->set_process(new_name);
    });
}


} // namespace stat_models
} // namespace hh_analysis
