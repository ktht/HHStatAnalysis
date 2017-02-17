/*! Definition of common uncertaintainties.
This file is part of https://github.com/cms-hh/StatAnalysis. */

#pragma once

#include "HHStatAnalysis/StatModels/interface/Uncertainty.h"

#define UNC(name, cor_range, distr_type, ...) \
    static const GlobalUncertainty& name() { \
        static const GlobalUncertainty u(#name, CorrelationRange::cor_range, \
                                         UncDistributionType::distr_type, ##__VA_ARGS__); \
        return u; \
    }

namespace hh_analysis {
namespace stat_models {
namespace Run2_2016 {

struct CommonUncertainties {

    // LHC uncertainties
    UNC(lumi, LHC, lnN, 1.058)
    UNC(cr_DiBoson, LHC, lnN, 1.1)
    UNC(cr_TTbar, LHC, lnN, 1.05)
    UNC(cr_SM_HH, LHC, lnN, 1.08)
    UNC(br_SM_H_bb, LHC, lnN, 1.0065)
    UNC(br_SM_H_tautau, LHC, lnN, 1.0117)

    // CMS uncertainties
    UNC(scale_j, Experiment, lnN)
    UNC(res_j, Experiment, shape)
    UNC(scale_b, Experiment, lnN, 1.02)
    UNC(eff_btag, Experiment, lnN)
    UNC(eff_e, Experiment, lnN)
    UNC(eff_mu, Experiment, lnN)
    UNC(eff_tau, Experiment, lnN)
    UNC(scale_tau, Experiment, shape)
    UNC(topPt, Experiment, shape)
};

} // namespace Run2_2016
} // namespace stat_models
} // namespace hh_analysis

#undef UNC
