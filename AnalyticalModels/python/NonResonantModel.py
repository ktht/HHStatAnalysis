# Provides scale factors for event reweighting based on an analytical model.
# This file is part of https://github.com/cms-hh/HHStatAnalysis.
# compiling

import ROOT
import numpy as np
from array import array
import matplotlib
import matplotlib.pyplot as plt

class NonResonantModel:
    def __init__(self):
        # read coefficients from the input file here
        # to store coefficients use self.
        """
        self.effSM = np.zeros((3,13))
        self.effSum = np.zeros((3,13))
        self.MHH = np.zeros((3,13))
        self.COSTS = np.zeros((3,13))
        self.A1 = np.zeros((3,13))
        self.A2 = np.zeros((3,13))
        self.A3 = np.zeros((3,13))
        self.A4 = np.zeros((3,13))
        self.A5 = np.zeros((3,13))
        self.A6 = np.zeros((3,13))
        self.A7 = np.zeros((3,13))
        self.A8 = np.zeros((3,13))
        self.A9 = np.zeros((3,13))
        self.A10 = np.zeros((3,13))
        self.A11 = np.zeros((3,13))
        self.A12 = np.zeros((3,13))
        self.A13 = np.zeros((3,13))
        self.A14 = np.zeros((3,13))
        self.A15 = np.zeros((3,13))
        """
        self.effSM = np.zeros((3,15))
        self.effSum = np.zeros((3,15))
        self.MHH = np.zeros((3,15))
        self.COSTS = np.zeros((3,15))
        self.A1 = np.zeros((3,15))
        self.A2 = np.zeros((3,15))
        self.A3 = np.zeros((3,15))
        self.A4 = np.zeros((3,15))
        self.A5 = np.zeros((3,15))
        self.A6 = np.zeros((3,15))
        self.A7 = np.zeros((3,15))
        self.A8 = np.zeros((3,15))
        self.A9 = np.zeros((3,15))
        self.A10 = np.zeros((3,15))
        self.A11 = np.zeros((3,15))
        self.A12 = np.zeros((3,15))
        self.A13 = np.zeros((3,15))
        self.A14 = np.zeros((3,15))
        self.A15 = np.zeros((3,15))
        print "initialize"

    # Declare the function
    def functionGF(self, kl,kt,c2,cg,c2g,A): return A[0]*kt**4 + A[1]*c2**2 + (A[2]*kt**2 + A[3]*cg**2)*kl**2  + A[4]*c2g**2 + ( A[5]*c2 + A[6]*kt*kl )*kt**2  + (A[7]*kt*kl + A[8]*cg*kl )*c2 + A[9]*c2*c2g  + (A[10]*cg*kl + A[11]*c2g)*kt**2+ (A[12]*kl*cg + A[13]*c2g )*kt*kl + A[14]*cg*c2g*kl

    def ReadCoefficients(self,inputFileName) : #,effSM,MHH,COSTS,A1,A3,A7):
        # here you should return TH2D histogram with BSM/SM coefficientes to calculate the scale factors for m_hh vs. cos_theta_star
        # loop over events and efficency calculation will be channel-dependent, so corresponding code
        # should go to the other file
        #filne = "coefficientsByBin_klkt.txt"
        f = open(inputFileName, 'r+')
        lines = f.readlines() # get all lines as a list (array)
        # Read coefficients by bin
        countercost=0
        countermhh=0
        for line in  lines:
          l = []
          tokens = line.split()
          for token in tokens:
              num = ""
              num_char = "."
              num2 = "e"
              num3 = "-"
              for char in token: 
                  if (char.isdigit() or (char in num_char) or (char in num2) or (char in num3)): num = num + char
              try: l.append(float(num))
              except ValueError: pass
          self.MHH[countercost][countermhh] = l[1] 
          self.COSTS[countercost][countermhh] = l[2] 
          self.effSM[countercost][countermhh] = l[3]/10000. # in units of 10k events
          self.effSum[countercost][countermhh] = l[4]/10000. # in units of 10k events # 12 JHEP benchmarks 
          # Just for testing purposes the above contains the number of events by bin from an ensenble of events 
          # calculated from the 12 benchmarks defined in 1507.02245v4 (JHEP version) each one with 100k events
          self.A1[countercost][countermhh] = l[5]
          self.A2[countercost][countermhh] = l[6]
          self.A3[countercost][countermhh] = l[7]
          self.A4[countercost][countermhh] = l[8]
          self.A5[countercost][countermhh] = l[9]
          self.A6[countercost][countermhh] = l[10]
          self.A7[countercost][countermhh] = l[11]
          self.A8[countercost][countermhh] = l[12]
          self.A9[countercost][countermhh] = l[13]
          self.A10[countercost][countermhh] = l[14]
          self.A11[countercost][countermhh] = l[15]
          self.A12[countercost][countermhh] = l[16]
          self.A13[countercost][countermhh] = l[17]
          self.A14[countercost][countermhh] = l[18]
          self.A15[countercost][countermhh] = l[19]
          countercost+=1
          if countercost == 3 :
             countercost=0
             countermhh+=1
        f.close()
        # and at the end of the function return it
        print "Stored coefficients by bin"

    def getNormalization(self,kl, kt,c2,cg,c2g,HistoAllEventsName):
      fileHH=ROOT.TFile(HistoAllEventsName) 
      HistoAllEvents = fileHH.Get("SumV0_AnalyticalBinExt")   
      sumOfWeights = 0 
      A13tev = [2.09078, 10.1517, 0.282307, 0.101205, 1.33191, -8.51168, -1.37309, 2.82636, 1.45767, -4.91761, -0.675197, 1.86189, 0.321422, -0.836276, -0.568156]
      sumW=0
      sumW2=0
      sumSM=0
      #HistW= float((1000.-250.)*2)
      print ("Nbins", HistoAllEvents.GetNbinsX(),HistoAllEvents.GetNbinsY()) 
      for binmhh in range (0,HistoAllEvents.GetNbinsX()) :
         WX = HistoAllEvents.GetXaxis().GetBinWidth(binmhh+1)
         for bincost in range (0,HistoAllEvents.GetNbinsY()) :
            WY = HistoAllEvents.GetYaxis().GetBinWidth(bincost+1)
            A = [self.A1[bincost][binmhh],self.A2[bincost][binmhh],self.A3[bincost][binmhh],\
               self.A4[bincost][binmhh],self.A5[bincost][binmhh],self.A6[bincost][binmhh],\
               self.A7[bincost][binmhh],self.A8[bincost][binmhh],self.A9[bincost][binmhh],\
               self.A10[bincost][binmhh],self.A11[bincost][binmhh],self.A12[bincost][binmhh],\
               self.A13[bincost][binmhh],self.A14[bincost][binmhh],self.A15[bincost][binmhh]]
            #if HistoAllEvents.GetBinContent(binmhh,bincost) > 0 : 
            sumOfWeights+=float(self.effSM[bincost][binmhh]*self.functionGF(kl,kt,c2,cg,c2g,A)/self.functionGF(kl,kt,c2,cg,c2g,A13tev))
            sumW+=self.functionGF(kl,kt,c2,cg,c2g,A13tev)
            sumW2+=self.functionGF(kl,kt,c2,cg,c2g,A)
            sumSM+=self.effSM[bincost][binmhh]
      fileHH.Close()
      return float(sumOfWeights)

    # distribute the calculated GenMHH and CostS in the bins numbering  (matching the coefficientsByBin_klkt.txt)
    def getScaleFactor(self,mhh , cost,kl, kt,c2,cg,c2g, effSumV0,norm) : # ,effSM,MHH,COSTS,A1,A3,A7):   
       """
       binGenMHH = [250.,270.,300.,330.,360.,390., 420.,450.,500.,550.,600.,700.,800.,1000.]
       binGenCostS  = [ -1., -0.55,0.55,1.  ]
       # determine from which bin the event belong
       binmhh = 0
       bincost = 0
       for ii in range (0,13) : 
         if mhh > binGenMHH[12-ii] : 
            binmhh = 12-ii 
            break
       for ii in range (0,3) : 
         if cost > binGenCostS[2-ii] : 
            bincost = 2-ii
            break
       """
       binGenMHH = [245.,270.,300.,330.,360.,390., 420.,450.,500.,550.,600.,700.,800.,1000.,1500.,50000]
       binGenCostS  = [ -1., -0.55,0.55,1.  ]
       # determine from which bin the event belong
       binmhh = 0
       bincost = 0
       for ii in range (0,15) : 
         if mhh >= binGenMHH[14-ii] : 
            binmhh = 14-ii 
            break
       for ii in range (0,3) : 
         if cost >= binGenCostS[2-ii] : 
            bincost = 2-ii
            break
       # calculate the weight
       A13tev = [2.09078, 10.1517, 0.282307, 0.101205, 1.33191, -8.51168, -1.37309, 2.82636, 1.45767, -4.91761, -0.675197, 1.86189, 0.321422, -0.836276, -0.568156]
       #if effSum > 0 and A1 > 0: 
       if effSumV0 > 0 :
          A = [self.A1[bincost][binmhh],self.A2[bincost][binmhh],self.A3[bincost][binmhh],\
               self.A4[bincost][binmhh],self.A5[bincost][binmhh],self.A6[bincost][binmhh],\
               self.A7[bincost][binmhh],self.A8[bincost][binmhh],self.A9[bincost][binmhh],\
               self.A10[bincost][binmhh],self.A11[bincost][binmhh],self.A12[bincost][binmhh],\
               self.A13[bincost][binmhh],self.A14[bincost][binmhh],self.A15[bincost][binmhh]]
          effBSM = float(self.effSM[bincost][binmhh]*self.functionGF(kl,kt,c2,cg,c2g,A)/self.functionGF(kl,kt,c2,cg,c2g,A13tev))
          #if v1 ==0 : CalcWeight = effBSM/float(effSum[bincost][binmhh]) # ==> JHEP sum in denominator
          CalcWeight = (effBSM/float(effSumV0))/norm # ==> V0 sum in denominator (Moriond 2016)
          return CalcWeight
       else : return 0


    ### only to read the text files to test

    def ReadLine(self,line, countline,Px,Py,Pz,En) :
            l = []
            tokens = line.split()
            for token in tokens:
                num = ""
                num_char = "."
                num2 = "e"
                num3 = "-"
                for char in token:     
                    if (char.isdigit() or (char in num_char) or (char in num2) or (char in num3)): num = num + char
                try: l.append(float(num))
                except ValueError: pass
            if countline < 2 :
               Px[countline] = l[1] 
               Py[countline] = l[2]
               Pz[countline] = l[3]
               En[countline] = l[4]
            #return countline

    def CalculateMhhCost(self,mhhcost,countline,Px,Py,Pz,En) :
               # calculate reweigthing 
               if abs(Px[0])!= abs(Px[1]) : print "error parsing ascii file"
               P1 = ROOT.TLorentzVector()
               P1.SetPxPyPzE(Px[0],Py[0],Pz[0],En[0])    
               P2 = ROOT.TLorentzVector()
               P1.SetPxPyPzE(Px[1],Py[1],Pz[1],En[1])
               SUM = ROOT.TLorentzVector()
               SUM.SetPxPyPzE(Px[0]+Px[1],Py[0]+Py[1],Pz[0]+Pz[1],En[0]+En[1])
               mhhcost[0]=SUM.M()
               P1boost = P1
               P1boost.Boost(-SUM.BoostVector())
               mhhcost[1] = float(P1boost.CosTheta())
               mhhcost[2] = float(P1.Pt())
               mhhcost[3] = float(SUM.Pt())

    def LoadTestEvents(self,CalcMhhTest,CalcCostTest,CalcPtHTest,CalcPtHHTest,filne) :
       counteventSM = 0
       Px = np.zeros((2)) 
       Py = np.zeros((2)) 
       Pz = np.zeros((2)) 
       En = np.zeros((2)) 
       f = open(filne, 'r+')
       lines = f.readlines() # get all lines as a list (array)
       countline = 0 # particuliarity of the text file with events = each 2 lines are one event there
       for line in  lines:
             self.ReadLine(line, countline,Px,Py,Pz,En)
             #print countline
             countline+=1
             mhhcost= [0,0,0,0] # to store [mhh , cost] of that event
             if countline==2 : # if read 2 lines 
                self.CalculateMhhCost(mhhcost,countline,Px,Py,Pz,En) # ==> adapt to your input 
                countline=0
                CalcMhhTest[counteventSM] = float(mhhcost[0])
                CalcCostTest[counteventSM] = float(mhhcost[1])
                CalcPtHTest[counteventSM] = float(mhhcost[2])
                CalcPtHHTest[counteventSM] = float(mhhcost[3])
                counteventSM+=1

    def FindBin(self,mhh,cost,histfile) :
       fileHH=ROOT.TFile(histfile) #Distros_5p_SM3M_sumBenchJHEP_13TeV.root") # do the histo from V0
       sumJHEPAnalyticalBin = fileHH.Get("SumV0_AnalyticalBinExt")
       bmhh = histfile.GetXaxis().FindBin(mhh)
       bcost = histfile.GetYaxis().FindBin(cost)
       effSumV0 = sumJHEPAnalyticalBin.GetBinContent(bmhh,bcost) 
       fileHH.Close()
       #print (mhh,cost,bmhh,bcost,effSumV0)
       return effSumV0

    ###################################################
    # Draw the histograms
    #####################################################
    def plotting(self,kl,kt,c2,cg,c2g, CalcMhh,CalcCost,CalcPtH,CalcPtHH,CalcWeight,CalcMhhTest,CalcCostTest,CalcPtHTest,CalcPtHHTest,drawtest):

      print "Plotting test histograms"
      bin_size = 20; min_edge = 260; max_edge = 1000
      N = (max_edge-min_edge)/bin_size; Nplus1 = N + 1
      bin_list = np.linspace(min_edge, max_edge, Nplus1)
      plt.xlim(min_edge, max_edge)
      if drawtest==1 : plt.hist(CalcMhhTest, bin_list , normed=1,  histtype='bar', label='simulated', fill=False, color= 'g', edgecolor='g', lw=5)
      plt.hist(CalcMhh, bin_list, weights=CalcWeight , normed=1, histtype='bar', label='reweigted', fill=False, color= 'r', edgecolor='r', lw=2)
      plt.legend(loc='upper right')
      plt.title(" In  kl =="+str(kl)+", kt =="+str(kt)+", c2 =="+str(c2)+", cg =="+str(cg)+", c2g ==" +str(c2g) )
      plt.xlabel("Mhh (GeV)")
      plt.ylabel("a.u.")
      plt.savefig("Mhh_kl_"+str(kl)+"_kt_"+str(kt)+"_c2_"+str(c2)+"_cg_"+str(cg)+"_c2g_" +str(c2g)+".pdf")
      plt.cla()   # Clear axis
      plt.clf()   # Clear figure
      plt.close() 
      bin_size = 0.1; min_edge = -1; max_edge = 1
      N = (max_edge-min_edge)/bin_size; Nplus1 = N + 1
      bin_list = np.linspace(min_edge, max_edge, Nplus1)
      plt.xlim(min_edge, max_edge)
      if drawtest==1 : plt.hist(CalcCostTest, bin_list , normed=1,  histtype='bar', label='simulated',fill=False, color= 'g', edgecolor='g', lw=5)
      plt.hist(CalcCost, bin_list, weights=CalcWeight , normed=1, histtype='bar', label='reweigted', fill=False, color= 'r', edgecolor='r', lw=2)
      plt.title(" In  kl =="+str(kl)+", kt =="+str(kt)+", c2 =="+str(c2)+", cg =="+str(cg)+", c2g ==" +str(c2g) )
      plt.legend(loc='upper right')
      plt.xlabel("Cost*")
      plt.ylabel("a.u.")
      plt.savefig("CostS_kl_"+str(kl)+"_kt_"+str(kt)+"_c2_"+str(c2)+"_cg_"+str(cg)+"_c2g_" +str(c2g)+".pdf")
      plt.cla()   # Clear axis
      plt.clf()   # Clear figure
      plt.close() 
      bin_size = 10; min_edge = 0; max_edge = 400
      N = (max_edge-min_edge)/bin_size; Nplus1 = N + 1
      bin_list = np.linspace(min_edge, max_edge, Nplus1)
      plt.xlim(min_edge, max_edge)
      if drawtest==1 : plt.hist(CalcPtHTest, bin_list , normed=1,  histtype='bar', label='simulated', fill=False, color= 'g', edgecolor='g', lw=5)
      plt.hist(CalcPtH, bin_list, weights=CalcWeight , normed=1, histtype='bar', label='reweigted', fill=False, color= 'r', edgecolor='r', lw=2)
      plt.legend(loc='upper right')
      plt.title(" In  kl =="+str(kl)+", kt =="+str(kt)+", c2 =="+str(c2)+", cg =="+str(cg)+", c2g ==" +str(c2g) )
      plt.xlabel("Pt h (GeV)")
      plt.ylabel("a.u.")
      plt.savefig("PtH_kl_"+str(kl)+"_kt_"+str(kt)+"_c2_"+str(c2)+"_cg_"+str(cg)+"_c2g_" +str(c2g)+".pdf")
      plt.cla()   # Clear axis
      plt.clf()   # Clear figure
      plt.close() 
      bin_size = 10; min_edge = 0; max_edge = 100
      N = (max_edge-min_edge)/bin_size; Nplus1 = N + 1
      bin_list = np.linspace(min_edge, max_edge, Nplus1)
      plt.xlim(min_edge, max_edge)
      if drawtest==1 : plt.hist(CalcPtHHTest, bin_list , normed=1,  histtype='bar', label='simulated', fill=False,  color= 'g', edgecolor='g', lw=5)
      plt.hist(CalcPtHH, bin_list, weights=CalcWeight , normed=1, histtype='bar', label='reweigted', fill=False, color= 'r', edgecolor='r', lw=2)
      plt.legend(loc='upper right')
      plt.title(" In  kl =="+str(kl)+", kt =="+str(kt)+", c2 =="+str(c2)+", cg =="+str(cg)+", c2g ==" +str(c2g) )
      plt.xlabel("Pt hh (GeV)")
      plt.ylabel("a.u.")
      plt.savefig("PtHH_kl_"+str(kl)+"_kt_"+str(kt)+"_c2_"+str(c2)+"_cg_"+str(cg)+"_c2g_" +str(c2g)+".pdf")
      plt.cla()   # Clear axis
      plt.clf()   # Clear figure
      plt.close() 
