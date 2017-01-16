/*! Implementation of the HH StatModel factory.
This file is part of https://github.com/cms-hh/HHStatAnalysis. */

#include <unordered_map>

#include "HHStatAnalysis/StatModels/interface/StatModelFactory.h"
#include "HHStatAnalysis/StatModels/interface/bbtautau_Resonant.h"
#include "HHStatAnalysis/Core/interface/exception.h"

namespace hh_analysis {
namespace stat_models {

namespace {

using ProducerFn = StatModelPtr (*)(const StatModelDescriptor& model_desc);
using ProducerFnMap = std::unordered_map<std::string, ProducerFn>;

template<typename Model>
StatModelPtr DefaultProducer(const StatModelDescriptor& model_desc)
{
    return StatModelPtr(new Model(model_desc));
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

StatModelPtr StatModelFactory::Make(const StatModelDescriptor& model_desc)
{
    if(!GetProducerFunctions().count(model_desc.stat_model)) {
        std::ostringstream ss;
        ss << "Unknown model name '" << model_desc.stat_model << "'.\n";
        ReportAvailableModelNames(ss);
        throw exception(ss.str());
    }
    return GetProducerFunctions().at(model_desc.stat_model)(model_desc);
}

} // namespace stat_models
} // namespace hh_analysis
