#pragma once

#include <unordered_set>
#include <unordered_map>
#include "HH_Model.h"

namespace hh_limits {

class HH_ModelFactory {
public:
    using ModelNameSet = std::unordered_set<std::string>;
    static HH_ModelPtr Make(const std::string& model_name);
    static const ModelNameSet& AvailableModelNames();
    static void ReportAvailableModelNames(std::ostream& os);

private:
    ~HH_ModelFactory() {}
};

} // namespace hh_limits
