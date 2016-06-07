/*! Implementation of the HH StatModel factory.
This file is part of https://github.com/cms-hh/StatAnalysis. */

#include <unordered_map>

#include "HHStatAnalysis/StatModels/interface/StatModelFactory.h"
#include "HHStatAnalysis/StatModels/interface/bbtautau_Resonant.h"
#include "HHStatAnalysis/Core/interface/exception.h"

namespace hh_analysis {
namespace stat_models {

namespace {

using ProducerFn = StatModelPtr (*)();
using ProducerFnMap = std::unordered_map<std::string, ProducerFn>;

template<typename Model>
StatModelPtr DefaultProducer()
{
    return StatModelPtr(new Model());
}

const ProducerFnMap& GetProducerFunctions()
{
    static const ProducerFnMap producers {
        { "bbtautau_resonant", &DefaultProducer<bbtautau_Resonant> }
    };
    return producers;
}

} // anonymous namespace

const StatModelFactory::ModelNameSet& StatModelFactory::AvailableModelNames()
{
    static ModelNameSet model_names;
    if(!model_names.size()) {
        std::transform(GetProducerFunctions().begin(), GetProducerFunctions().end(),
                       std::inserter(model_names, model_names.end()),
                       [](const ProducerFnMap::value_type& entry) { return entry.first; });
    }
    return model_names;
}

void StatModelFactory::ReportAvailableModelNames(std::ostream& os)
{
    if(!AvailableModelNames().size()) {
        os << "No HH models is available.";
        return;
    }
    auto iter = AvailableModelNames().begin();
    os << "Available HH unc model names: " << *iter++;
    for(; iter != AvailableModelNames().end(); ++iter)
        os << ", " << *iter;
    os << ".";
}

StatModelPtr StatModelFactory::Make(const std::string& model_name)
{
    if(!GetProducerFunctions().count(model_name)) {
        std::ostringstream ss;
        ss << "Unknown model name '" << model_name << "'.\n";
        ReportAvailableModelNames(ss);
        throw exception(ss.str());
    }
    return GetProducerFunctions().at(model_name)();
}

} // namespace stat_models
} // namespace hh_analysis
