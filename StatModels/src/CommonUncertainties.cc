/*! Definition of common uncertaintainties.
This file is part of https://github.com/cms-hh/StatAnalysis. */

#include "HHStatAnalysis/StatModels/interface/CommonUncertainties.h"

namespace hh_analysis {

const Uncertainty CommonUncertainties::lumi{CorrelationRange::LHC, UncDistributionType::lnN, "lumi"};
const Uncertainty CommonUncertainties::cr_DiBoson{CorrelationRange::LHC, UncDistributionType::lnN, "cr_DiBoson"};
const Uncertainty CommonUncertainties::cr_TTbar{CorrelationRange::LHC, UncDistributionType::lnN, "cr_TTbar"};

const Uncertainty CommonUncertainties::scale_j{CorrelationRange::Experiment, UncDistributionType::lnN, "scale_j"};
const Uncertainty CommonUncertainties::scale_b{CorrelationRange::Experiment, UncDistributionType::lnN, "scale_b"};
const Uncertainty CommonUncertainties::eff_btag{CorrelationRange::Experiment, UncDistributionType::lnN, "eff_btag"};

const Uncertainty CommonUncertainties::eff_e{CorrelationRange::Experiment, UncDistributionType::lnN, "eff_e"};
const Uncertainty CommonUncertainties::eff_mu{CorrelationRange::Experiment, UncDistributionType::lnN, "eff_mu"};
const Uncertainty CommonUncertainties::eff_tau{CorrelationRange::Experiment, UncDistributionType::lnN, "eff_tau"};
const Uncertainty CommonUncertainties::scale_tau{CorrelationRange::Experiment, UncDistributionType::shape, "scale_tau"};
const Uncertainty CommonUncertainties::topPt{CorrelationRange::Experiment, UncDistributionType::shape, "topPt"};


const CommonUncertainties::UncValueMap CommonUncertainties::global_unc_values = {
    { CommonUncertainties::lumi, 1.058 },
    { CommonUncertainties::cr_DiBoson, 1.1 },
    { CommonUncertainties::cr_TTbar, 1.05 },

    { CommonUncertainties::scale_b, 1.02 },
};

void Uncertainty::ApplyGlobal(ch::CombineHarvester& cb, const std::vector<std::string>& processes,
                              const std::vector<std::string>& processes_2,
                              const std::vector<std::string>& processes_3) const
{
    Apply(cb, CommonUncertainties::GetGlobalValue(*this), processes, processes_2, processes_3);
}

} // namespace hh_analysis

