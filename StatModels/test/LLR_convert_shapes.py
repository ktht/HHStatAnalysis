#!/usr/bin/env python
# Rearrange LLR bbtautau shape histograms into histograms with the common naming convention.
# This file is part of https://github.com/cms-hh/HHStatAnalysis.

import os
import re
import argparse
import glob
from ROOT import TFile

parser = argparse.ArgumentParser(description='Apply common naming convention to LLR bbtautau shape histograms.',
                    formatter_class = lambda prog: argparse.HelpFormatter(prog, max_help_position=30, width=90))
parser.add_argument('--input', required=True, dest='input_path', type=str, metavar='INPUT_PATH',
                    help="path were located original histograms")
parser.add_argument('--output', required=True, dest='output', type=str, metavar='OUTPUT_FILE',
                    help="path were to store limits")
parser.add_argument('--analysis', required=True, dest='analysis', type=str, metavar='OUTPUT_FILE',
                    help="res_lm, res_hm or nonres")
args = parser.parse_args()

def LoadHistogram(file, hist_name, raise_exception = True):
    hist =  file.Get(hist_name)
    if hist == None:
        if raise_exception:
            raise RuntimeError('Histogram "{}" not found in "{}".'.format(hist_name, file.GetName()))
        return None
    return hist.Clone()

def NumToName(x):
    s = str(x)
    s = re.sub('-', 'm', s)
    return re.sub(r'\.', 'p', s)


output = TFile(args.output, 'RECREATE')
os.chdir(args.input_path)

file_name_pattern = '{}.root'
hist_name_pattern = '{}_{}_{}_{}'

signal_region = 'SR'

bins = {
    'data_obs'  : [ [ 'data_obs' ], [] ],
    'TT'        : [ [ 'TT' ], [] ],
    'DY_0b'     : [ [ 'DY0b' ], [] ],
    'DY_1b'     : [ [ 'DY1b' ], [] ],
    'DY_2b'     : [ [ 'DY2b' ], [] ],
    'QCD'       : [ [ 'QCD' ], [] ],
    'W'         : [ [ 'WJets' ], [] ],
    'tW'        : [ [ 'TWtop', 'TWantitop' ], [] ],
    'VV'        : [ [ 'WWToLNuQQ', 'WZTo1L1Nu2Q', 'WZTo1L3Nu', 'WZTo2L2Q', 'ZZTo2L2Q' ], [] ]
}

shape_variable = None

if args.analysis == 'res_lm' or args.analysis == 'res_hm':
    shape_variable = 'HHKin_mass_raw'
    masses = [ 250, 260, 270, 280, 300, 320, 340, 350, 400, 500, 550, 650, 700, 750, 800, 900 ]
    out_name_source = lambda bin_name, point: '{}_M{}'.format(bin_name, point)
    LLR_name_source = lambda sample, point: '{}{}'.format(sample, point)
    bins['ggRadion_hh_ttbb'] = [ [ 'Radion' ], [ out_name_source, LLR_name_source, masses ] ]
elif args.analysis == 'nonres':
    shape_variable = 'MT2'
    shift = 20
    k_lambda = range(-20, 32)
    out_name_source = lambda bin_name, point: '{}_kl_{}'.format(bin_name, NumToName(point))
    LLR_name_source = lambda sample, point: '{}{}'.format(sample, point + shift)
    bins['ggh_hh_ttbb'] = [ [ 'lambdarew' ], [ out_name_source, LLR_name_source, k_lambda ] ]
else:
    raise RuntimeError("Unsupported analysis '{}'".format(args.analysis))

eMu_Tau_categories = None
tauTau_categories = { 'res1b': 's1b1jresolvedMcut', 'res2b': 's2b0jresolvedMcut',
                      'boosted': 'sboostedLLMcut' }

if args.analysis == 'res_lm' or args.analysis == 'nonres':
    eMu_Tau_categories = { 'res1b': 's1b1jresolvedMcutlmr90', 'res2b': 's2b0jresolvedMcutlmr90',
                           'boosted': 'sboostedLLMcut' }
else:
    eMu_Tau_categories = { 'res1b': 's1b1jresolvedMcuthmr90', 'res2b': 's2b0jresolvedMcuthmr90',
                            'boosted': 'sboostedLLMcut' }

channels = {
    'eTau'      : [ 'ETau', eMu_Tau_categories ],
    'muTau'     : [ 'MuTau', eMu_Tau_categories ],
    'tauTau'    : [ 'TauTau', tauTau_categories ]
}

for channel,channel_desc in channels.iteritems():
    LLR_channel = channel_desc[0]
    categories = channel_desc[1]
    print '{} -> {}'.format(LLR_channel, channel)
    for category,LLR_category in categories.iteritems():
        output_dir_name = '{}_{}'.format(channel, category)
        print '\tprocessing {}'.format(category)
        output_dir = output.mkdir(output_dir_name)
        file_name = file_name_pattern.format(LLR_channel)
        input_file = TFile(file_name, 'OPEN')

        for bin,bin_desc in bins.iteritems():
            LLR_sample_names = bin_desc[0]
            points_desc = bin_desc[1]
            hist_names = {}
            if len(points_desc) == 0:
                hist_names[bin] = LLR_sample_names
            else:
                for point in points_desc[2]:
                    hist_name = points_desc[0](bin, point)
                    hist_names[hist_name] = []
                    for sample_name in LLR_sample_names:
                        LLR_sample_name = points_desc[1](sample_name, point)
                        hist_names[hist_name].append(LLR_sample_name)
            for hist_name,sample_names in hist_names.iteritems():
                hist = None
                print '\t\tcreating {}'.format(hist_name)
                for LLR_sample_name in sample_names:
                    LLR_hist_name = hist_name_pattern.format(LLR_sample_name, LLR_category,
                                                             signal_region, shape_variable)
                    print '\t\t\t {}'.format(LLR_hist_name)
                    new_hist = LoadHistogram(input_file, LLR_hist_name, True)
                    # if new_hist == None: continue
                    if hist == None:
                        hist = new_hist
                    else:
                        hist.Add(new_hist)
                if hist == None:
                    raise RuntimeError('Can not create empty histogram for {} {}'.format(channel, category))
                output_dir.WriteTObject(hist, hist_name, 'Overwrite')
        input_file.Close()

output.Close()
