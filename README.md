# CMS HH limit extraction and combination package

The purpose of this package is to have centralized CMS HH code base for the limit extraction and combination between the different channels for both model independent and model dependent cases.

This package is based on the tools, code and interfaces provided by [CombineHarvester](https://github.com/cms-analysis/CombineHarvester) package and [CMS Higgs Combination toolkit](https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit).

## How to install

The installation steps follows guidelines from CombineHarvester (http://cms-analysis.github.io/CombineHarvester/) and CMS combine (https://twiki.cern.ch/twiki/bin/viewauth/CMS/SWGuideHiggsAnalysisCombinedLimit/) documentations:

```shell
export SCRAM_ARCH=slc6_amd64_gcc491
cmsrel CMSSW_7_4_7
cd CMSSW_7_4_7/src
cmsenv
git clone https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git HiggsAnalysis/CombinedLimit
cd HiggsAnalysis/CombinedLimit
git checkout v6.2.1
cd ../..
git clone https://github.com/cms-analysis/CombineHarvester.git CombineHarvester
cd CombineHarvester
git checkout CombineHarvester-v16.2.1
cd ..
git clone -o upstream git@github.com:cms-hh/HHStatAnalysis.git HHStatAnalysis
git clone -o upstream git@github.com:cms-hh/Resources.git HHStatAnalysis/Resources
scram b -j4
```

## How to run

For details about how to run tools provided by CombineHarvester (e.g. combineTool.py, plotLimits.py), please, see its documentation: http://cms-analysis.github.io/CombineHarvester/.


### How to run limits for a single channel

- Create datacards:
```shell
create_hh_datacards --unc-model UNC_MODEL_NAME --shapes-path SHAPES_PATH --output-path OUTPUT_PATH
```
where *UNC_MODEL_NAME* is the name of the module that implements an uncertainty model for the given channel (e.g. "bbtautau_resonant"), *SHAPES_PATH* - path to the ROOT files with the signal, data and background shapes, *OUTPUT_PATH* - path where to store datacards.

- Create workspace:
```shell
cd OUTPUT_PATH
combineTool.py -M T2W -i */* -o workspace.root --parallel 8
```

- Run limits:
```shell
combineTool.py -M Asymptotic -d */*/workspace.root --there -n .limit --parallel 8
```

- Collect all outputs:
```shell
combineTool.py -M CollectLimits */*/*.limit.* --use-dirs -o limits.json
```

- Produce plots:
```shell
plotLimits.py JSON_FILE --auto-style
```

## How to add new statistical model

To add new stat. model one should implement hh_analysis::stat_models::StatModel interface within new class inside HHStatAnalysis/StatModels package and add producer for this class into the producer map in GetProducerFunctions function implemented inside StatModelFactory.cc.
