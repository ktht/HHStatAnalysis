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
#include "HHStatAnalysis/StatModels/interface/StatTools.h"

namespace hh_analysis {
namespace stat_models {
namespace Run2_2016 {

const StatModel::v_str ttbb_base::ana_name = { "hh_ttbb" };
const StatModel::v_str ttbb_base::eras = { "13TeV" };
const std::string ttbb_base::bkg_TT = "TT";
const std::string ttbb_base::bkg_tW = "tW";
const std::string ttbb_base::bkg_W = "W";
const std::string ttbb_base::bkg_EWK = "EWK";
const std::string ttbb_base::bkg_ZH = "ZH";
const std::string ttbb_base::bkg_QCD = "QCD";
const std::string ttbb_base::bkg_DY_0b = "DY_0b";
const std::string ttbb_base::bkg_DY_1b = "DY_1b";
const std::string ttbb_base::bkg_DY_2b = "DY_2b";
const StatModel::v_str ttbb_base::bkg_DY = { bkg_DY_0b, bkg_DY_1b, bkg_DY_2b };
const StatModel::v_str ttbb_base::bkg_VV = { "WW", "WZ", "ZZ" };
const StatModel::v_str ttbb_base::bkg_pure_MC = analysis::tools::join(bkg_tW, bkg_VV, bkg_W, bkg_EWK, bkg_ZH);
const StatModel::v_str ttbb_base::bkg_MC = analysis::tools::join(bkg_pure_MC, bkg_DY, bkg_TT);
const StatModel::v_str ttbb_base::bkg_all = analysis::tools::join(bkg_MC, bkg_QCD);

ttbb_base::ttbb_base(const StatModelDescriptor& _desc, const std::string& input_file_name) :
    StatModel(_desc, input_file_name), signal_processes({ desc.signal_process }),
    all_mc_processes(ch::JoinStr({ signal_processes, bkg_MC })),
    all_processes(ch::JoinStr({ signal_processes, bkg_all }))
{
}

void ttbb_base::AddSystematics(ch::CombineHarvester& cb)
{
    using ch::syst::SystMap;
    using CU = CommonUncertainties;
    static constexpr double unc_thr = 0.005;

    CU::lumi().ApplyGlobal(cb, signal_processes, bkg_pure_MC, bkg_TT);
    CU::QCDscale_VV().ApplyGlobal(cb, bkg_VV);
    CU::QCDscale_EWK().ApplyGlobal(cb, bkg_EWK);
    CU::QCDscale_ttbar().ApplyGlobal(cb, bkg_TT, bkg_tW);

    CU::scale_j().Apply(cb, all_mc_processes);
    CU::scale_b().ApplyGlobal(cb, signal_processes, bkg_pure_MC, bkg_TT);

    static constexpr double eff_b_unc = 0.02;
    CU::eff_b().Apply(cb, eff_b_unc, bkg_DY_1b, bkg_VV, bkg_EWK, bkg_ZH, bkg_tW);
    CU::eff_b().Apply(cb, eff_b_unc * std::sqrt(2.), signal_processes, bkg_DY_2b, bkg_TT);

    CU::eff_e().Channel("eTau").Apply(cb, CU::eff_e().up_value, all_mc_processes);
    CU::eff_m().Channel("muTau").Apply(cb, CU::eff_m().up_value, all_mc_processes);
    CU::eff_t().Channels({"eTau", "muTau"}).Apply(cb, CU::eff_t().up_value, all_mc_processes);
    CU::eff_t().Channel("tauTau").Apply(cb, CU::eff_t().up_value * std::sqrt(2.), all_mc_processes);
    CU::scale_t().Apply(cb, all_processes);

    CU::topPt().Apply(cb, bkg_TT);

    static constexpr size_t DYUncDim = 3;
    TMatrixD dy_unc_corr(DYUncDim, DYUncDim);
    dy_unc_corr[0][0] = dy_unc_corr[1][1] = dy_unc_corr[2][2] = 1.0;
    dy_unc_corr[0][1] = dy_unc_corr[1][0] = -0.321;
    dy_unc_corr[0][2] = dy_unc_corr[2][0] = 0.149;
    dy_unc_corr[1][2] = dy_unc_corr[2][1] = -0.449;

    TVectorD dy_sf_unc(DYUncDim);
    dy_sf_unc[0] = 0.0017;
    dy_sf_unc[1] = 0.016;
    dy_sf_unc[2] = 0.028;

    TVectorD dy_sf(DYUncDim);
    dy_sf[0] = 1.0472;
    dy_sf[1] = 1.186;
    dy_sf[2] = 1.037;

    auto dy_trans_inv = stat_tools::ComputeWhiteningMatrix(dy_unc_corr, dy_sf_unc).Invert();

    std::cout << "ttbb: whitened DY norm covariance matrix" << std::endl;
    dy_trans_inv.Print();

    for(size_t n = 0; n < DYUncDim; ++n) {
        std::ostringstream ss_unc_name;
        ss_unc_name << "DY_norm_unc_" << n;
        const Uncertainty DY_norm_unc(ss_unc_name.str(), CorrelationRange::Analysis, UncDistributionType::lnN);
        for(size_t k = 0; k < DYUncDim; ++k) {
            const double unc_value = dy_trans_inv[k][n] / dy_sf[k];
            if(std::abs(unc_value) >= unc_thr)
                DY_norm_unc.Apply(cb, unc_value, bkg_DY.at(k));
        }
    }

    static constexpr double qcd_ss_os_sf = 1.5;
    const Uncertainty qcd_norm("qcd_norm", CorrelationRange::Category, UncDistributionType::lnN);
    for(const auto& channel : desc.channels) {
        for(const auto& category : desc.categories) {
            const Yield qcd_yield = GetBackgroundYield(bkg_QCD, channel, category);
            const double ss_qcd_yield = qcd_yield.value / qcd_ss_os_sf;
            const double rel_error = 1 / std::sqrt(ss_qcd_yield);
            if(rel_error >= unc_thr)
                qcd_norm.Channel(channel).Category(category).Apply(cb, rel_error, bkg_QCD);
        }
    }

    const Uncertainty qcd_sf_unc("qcd_sf_unc", CorrelationRange::Category, UncDistributionType::lnN);
    qcd_sf_unc.Apply(cb, qcd_ss_os_sf - 1, bkg_QCD);
}

} // namespace Run2_2016
} // namespace stat_models
} // namespace hh_analysis
