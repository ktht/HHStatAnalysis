#include "hh-limits/Run2Models/interface/HH_ModelFactory.h"
#include "hh-limits/Run2Models/interface/bbtautau_Resonant.h"
#include "hh-limits/Run2Models/interface/exception.h"

namespace hh_limits {

namespace {

using ProducerFn = HH_ModelPtr (*)();
using ProducerFnMap = std::unordered_map<std::string, ProducerFn>;

template<typename Model>
HH_ModelPtr DefaultProducer()
{
    return HH_ModelPtr(new Model());
}

const ProducerFnMap& GetProducerFunctions()
{
    static const ProducerFnMap producers {
        { "bbtautau_resonant", &DefaultProducer<unc_models::bbtautau_Resonant> }
    };
    return producers;
}

} // anonymous namespace

const HH_ModelFactory::ModelNameSet& HH_ModelFactory::AvailableModelNames()
{
    static ModelNameSet model_names;
    if(!model_names.size()) {
        std::transform(GetProducerFunctions().begin(), GetProducerFunctions().end(),
                       std::inserter(model_names, model_names.end()),
                       [](const ProducerFnMap::value_type& entry) { return entry.first; });
    }
    return model_names;
}

void HH_ModelFactory::ReportAvailableModelNames(std::ostream& os)
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

HH_ModelPtr HH_ModelFactory::Make(const std::string& model_name)
{
    if(!GetProducerFunctions().count(model_name)) {
        std::ostringstream ss;
        ss << "Unknown model name '" << model_name << "'.\n";
        ReportAvailableModelNames(ss);
        throw analysis::exception(ss.str());
    }
    return GetProducerFunctions().at(model_name)();
}

} // namespace hh_limits
