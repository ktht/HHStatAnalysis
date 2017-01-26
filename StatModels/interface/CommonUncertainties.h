/*! Definition of common uncertaintainties.
This file is part of https://github.com/cms-hh/StatAnalysis. */

#pragma once

#include "Uncertainty.h"

namespace hh_analysis {

class CommonUncertainties {
public:
    using UncValueMap = std::map<Uncertainty, double>;

    // LHC uncertainties
    static const Uncertainty lumi, cr_DiBoson, cr_TTbar;

    // CMS uncertainties
    static const Uncertainty scale_j, scale_b, eff_btag, eff_e, eff_mu, eff_tau, scale_tau, topPt;

    static double GetGlobalValue(const Uncertainty& unc)
    {
        if(!global_unc_values.count(unc))
            throw analysis::exception("Global value for uncertainty descriptor '%1%' not found.") % unc.FullName();
        return global_unc_values.at(unc);
    }

private:
    static const UncValueMap global_unc_values;
};

} // namespace hh_analysis
