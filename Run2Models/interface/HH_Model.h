#pragma once

#include <memory>
#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"

namespace hh_limits {

class HH_Model {
public:
    using v_str = std::vector<std::string>;
    using v_double = std::vector<double>;

    static const v_str wildcard;

    virtual ~HH_Model() {}
    virtual void CreateDatacards(const std::string& shapes_path, const std::string& output_path) = 0;
};

using HH_ModelPtr = std::shared_ptr<HH_Model>;

} // namespace hh_limits
