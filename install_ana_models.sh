#!/bin/bash
# Install AnalyticalModels.
# This file is part of https://github.com/cms-hh/HHStatAnalysis.

mkdir HHStatAnalysis
cd HHStatAnalysis
git init
git remote add cms-hh git@github.com:cms-hh/HHStatAnalysis.git
git config core.sparsecheckout true
echo -e "AnalyticalModels/" >> .git/info/sparse-checkout
git pull cms-hh master
cd ..
