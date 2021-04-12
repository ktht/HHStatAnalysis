# Author Fabio Monti, IHEP Beijing
# 5 March 2021
# 
# This script provides an example of how to reweight a certain input 
# sample to a given output benchmark. 
#
# Before running the reweight you need the 2D distribution in (mHH-gen,costh-gen) 
# of your full input sample WITHOUT ANY SELECTION. In this example this is called 
# "histo_Nev". 
# NOTE: the binning for histo_Nev(mHH,costh) has to be the same defined in 
# NonResonantModelNLO!!!
#
# Then you can derive the reweight as:
#    reweight = [XS(mHH,costh|outputBM)/Nev(mHH,costh)] * [Nevtot/XStot(outputBM)]
#             
# NOTE: if you merge, e.g. with hadd, N reweighted samples each normalized to 1,
# then you need to scale by a factor N to preserve the overall normalization
#    
# The coefficients for the NLO reweight have been derived by P. Mandrik and originally 
# stored in https://github.com/pmandrik/VSEVA/tree/master/HHWWgg/reweight
#  

import  NonResonantModelNLO
import ROOT 
mymodel = NonResonantModelNLO.NonResonantModelNLO()
mymodel.ReadCoefficients("../data/pm_pw_NLO_Ais_13TeV_V2.txt") # local copy of coefficients

# event example
event_mHH=514
event_costhetaHH=0.5
event_weight=0.1234

# assume that I want to reweight a given input sample for benchmark XYZ
# to all the NLO benchmarks using the approach 2 as described above

inputevfile = ROOT.TFile("inputevdistribution.root")
histo_Nev = inputevfile.Get("inputev_benchmarkXYZ") # this is a TH2 histo
Nevtot = histo_Nev.Integral() 

#benchmarks as originally defined in JHEP04(2016)126 but at NLO 
for iBM in range(0,12):
    BMcouplings = mymodel.getBenchmark(iBM)
    kl, kt, c2, cg, c2g = BMcouplings[0], BMcouplings[1], BMcouplings[2], BMcouplings[3], BMcouplings[4]
    print "totalXS", mymodel.getTotalXS(kl, kt, c2, cg, c2g)
    print "diffXS(event_mHH,event_costhetaHH)", mymodel.getDifferentialXS2D(event_mHH, event_costhetaHH, kl, kt, c2, cg, c2g)
    print "diffXS(event_mHH)", mymodel.getDifferentialXSmHH(event_mHH, kl, kt, c2, cg, c2g)
    Nev = histo_Nev.GetBinContent( histo_Nev.FindBin(event_mHH, event_costhetaHH) )
    XS = mymodel.getDifferentialXS2D(event_mHH, event_costhetaHH, kl, kt, c2, cg, c2g)
    # before using the differential XS, scale it by the bin area to properly compare it with histo_Nev content 
    Noutputev = XS * mymodel.getmHHbinwidth(event_mHH) * mymodel.getcosthetabinwidth(event_costhetaHH) 
    XStot = mymodel.getTotalXS(kl, kt, c2, cg, c2g)
    reweight = event_weight * Noutputev/Nev * Nevtot/XStot
    print reweight
    print 

print "-----------------------"

#benchmark 8a as defined in JHEP09(2018)057
BMcouplings = mymodel.getBenchmark8a()
kl, kt, c2, cg, c2g = BMcouplings[0], BMcouplings[1], BMcouplings[2], BMcouplings[3], BMcouplings[4]
print "totalXS", mymodel.getTotalXS(kl, kt, c2, cg, c2g)
print "diffXS(event_mHH,event_costhetaHH)", mymodel.getDifferentialXS2D(event_mHH, event_costhetaHH, kl, kt, c2, cg, c2g)
print "diffXS(event_mHH)", mymodel.getDifferentialXSmHH(event_mHH, kl, kt, c2, cg, c2g)
print 
print "-----------------------"

# 7 more recent benchmarks as defined in JHEP03(2020)091
for iBM in range(0,7):
    BMcouplings = mymodel.getBenchmark2020(iBM)
    kl, kt, c2, cg, c2g = BMcouplings[0], BMcouplings[1], BMcouplings[2], BMcouplings[3], BMcouplings[4]
    print "totalXS", mymodel.getTotalXS(kl, kt, c2, cg, c2g)
    print "diffXS(event_mHH,event_costhetaHH)", mymodel.getDifferentialXS2D(event_mHH, event_costhetaHH, kl, kt, c2, cg, c2g)
    print "diffXS(event_mHH)", mymodel.getDifferentialXSmHH(event_mHH, kl, kt, c2, cg, c2g)
    print 
