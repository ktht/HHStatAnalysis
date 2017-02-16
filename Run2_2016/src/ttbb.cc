/*! Common part of stat models for the hh->bbtautau analyses.
This file is part of https://github.com/cms-hh/HHStatAnalysis. */

#include <iostream>
#include "CombineHarvester/CombineTools/interface/Systematics.h"
#include "CombineHarvester/CombineTools/interface/CardWriter.h"
#include "CombineHarvester/CombinePdfs/interface/MorphFunctions.h"
#include "HHStatAnalysis/Run2_2016/interface/ttbb.h"
#include "HHStatAnalysis/Core/interface/exception.h"
#include "HHStatAnalysis/Core/interface/Tools.h"
#include "HHStatAnalysis/Core/interface/TextIO.h"
#include "HHStatAnalysis/Core/interface/RootExt.h"
#include "HHStatAnalysis/Run2_2016/interface/CommonUncertainties.h"

namespace hh_analysis {
namespace stat_models {
namespace Run2_2016 {

const StatModel::v_str ttbb_base::ana_name = { "hh_ttbb" };
const StatModel::v_str ttbb_base::eras = { "13TeV" };
const StatModel::v_str ttbb_base::bkg_mc_processes = { "DY_0b", "DY_1b", "DY_2b", "TT", "tW", "VV", "W" };
const StatModel::v_str ttbb_base::bkg_data_driven_processes = { "QCD" };
const StatModel::v_str ttbb_base::bkg_all_processes = ch::JoinStr({ bkg_mc_processes, bkg_data_driven_processes });

ttbb_base::ttbb_base(const StatModelDescriptor& _desc) :
    StatModel(_desc), signal_processes({ desc.signal_process }),
    all_mc_processes(ch::JoinStr({ signal_processes, bkg_mc_processes }))
{
}

ch::Categories ttbb_base::GetChannelCategories(const std::string& channel)
{
    ch::Categories ch_categories;
    for(size_t n = 0; n < desc.categories.size(); ++n) {
        const std::string cat_name = desc.categories.at(n);
        const std::string name = boost::str(boost::format("%1%_%2%") % channel % cat_name);
        ch_categories.push_back({n, name});
    }
    return ch_categories;
}

void ttbb_base::AddSystematics(ch::CombineHarvester& cb)
{
    using ch::syst::SystMap;
    using CU = CommonUncertainties;

    CU::lumi().ApplyGlobal(cb, signal_processes, { "TT", "tW", "VV", "W" });
    CU::cr_DiBoson().ApplyGlobal(cb, { "VV" });
    CU::cr_TTbar().ApplyGlobal(cb, { "TT" });

    CU::scale_j().Apply(cb, 1.02, signal_processes);
    CU::scale_j().Apply(cb, 1.04, { "TT", "tW", "VV", "W" });
    CU::scale_b().Apply(cb, 1.02, signal_processes, { "TT", "tW", "VV", "W" });

    CU::eff_btag().Apply(cb, 1.02, signal_processes, { "DY_2b" });
    CU::eff_btag().Apply(cb, 1.03, { "TT", "DY_1b", "VV", "W" });
    CU::eff_btag().Apply(cb, 1.04, { "DY_0b", "tW" });

    CU::eff_e().Channel("eTau").Apply(cb, 1.03, all_mc_processes);
    CU::eff_mu().Channel("muTau").Apply(cb, 1.02, all_mc_processes);
    CU::eff_tau().Channels({"eTau", "muTau"}).Apply(cb, 1.06, all_mc_processes);
    CU::eff_tau().Channel("tauTau").Apply(cb, 1.08, all_mc_processes);
//    CU::scale_tau().Apply(cb, all_mc_processes);

//    CU::topPt().Apply(cb, { "TT" });

    const Uncertainty DY0b_sf("DY0b_sf", CorrelationRange::Analysis, UncDistributionType::lnN);
    const Uncertainty DY1b_sf("DY1b_sf", CorrelationRange::Analysis, UncDistributionType::lnN);
    const Uncertainty DY2b_sf("DY2b_sf", CorrelationRange::Analysis, UncDistributionType::lnN);
    DY0b_sf.Apply(cb, 1.01, { "DY_0b", "DY_1b", "DY_2b" });
    DY1b_sf.Apply(cb, 0.98, { "DY_0b" });
    DY1b_sf.Apply(cb, 0.80, { "DY_1b", "DY_2b" });
    DY2b_sf.Apply(cb, 1.01, { "DY_0b" });
    DY2b_sf.Apply(cb, 1.36, { "DY_1b", "DY_2b" });

    const Uncertainty qcd_norm("qcd_norm", CorrelationRange::Category, UncDistributionType::lnN);
//    const Uncertainty qcd_btag_relax("qcd_btag_relax", CorrelationRange::Category, UncDistributionType::shape);
    qcd_norm.Apply(cb, 1.06, { "QCD" });
//    qcd_btag_relax.Apply(cb, { "QCD" });
}

} // namespace Run2_2016
} // namespace stat_models
} // namespace hh_analysis
