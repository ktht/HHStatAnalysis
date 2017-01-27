#!/usr/bin/env python
# Merge bbgammagamma workspaces into a single file and add to them some missing information.
# This file is part of https://github.com/cms-hh/HHStatAnalysis.

import os
import re
import argparse
import glob
from ROOT import TFile, RooFit, RooRealVar

parser = argparse.ArgumentParser(
    description='Merge bbgammagamma workspaces into a single file and add to them some missing information.',
    formatter_class = lambda prog: argparse.HelpFormatter(prog, max_help_position=30, width=90))
parser.add_argument('--input', required=True, dest='input_path', type=str, metavar='INPUT_PATH',
                    help="path with input workspaces")
parser.add_argument('--output', required=True, dest='output', type=str, metavar='OUTPUT_FILE',
                    help="path were to store limits")
args = parser.parse_args()

output = TFile(args.output, 'RECREATE')
os.chdir(args.input_path)

singal_file_format = 'Radion_M{}/workspaces/hgg.mH125_8TeV.inputsig.root'
bkg_file_format = 'Radion_M{}/workspaces/hgg.inputbkg_8TeV.root'
signal_ws_format = 'ws_Radion_M{}'
bkg_ws_format = 'ws_bkg_M{}'

categories = [ 'cat0', 'cat1' ]
masses = [ 250, 260, 280, 300, 320, 350, 400, 450, 500 ]

root_dir_prefix = 'Radion_M'
ws_dir_name = 'workspaces'
card_file_name = 'datacards/hhbbgg_13TeV_DataCard.txt'
sources = [ [ 'Sig', 'hgg.mH125_8TeV.inputsig.root', 'ws_Radion_M{}' ],
            [ 'Bkg', 'hgg.inputbkg_8TeV.root', 'ws_bkg_M{}' ]
]

def split_line(line):
    return filter(lambda s: len(s) != 0, re.split(' |\t|\n', line))

def ExtractRatesFromDatacard(full_card_file_name):
    card_file = open(full_card_file_name, 'r')
    bins = []
    processes = []
    for line in card_file:
        if line[0:3] == 'bin':
            bins = split_line(line[3:])
        elif len(bins) != 0 and len(processes) == 0 and line[0:7] == 'process':
            processes = split_line(line[7:])
            if len(processes) != len(bins):
                raise RuntimeError('invalid datacard "{}"'.format(full_card_file_name))
        elif len(bins) != 0 and len(processes) != 0 and line[0:7] == 'process':
            continue
        elif len(bins) != 0 and len(processes) != 0 and line[0:4] == 'rate':
            rates = {}
            card_rates = split_line(line[4:])
            if len(card_rates) != len(bins):
                print bins
                print processes
                raise RuntimeError('invalid datacard "{}"'.format(full_card_file_name))
            for n in range(0, len(bins)):
                if not processes[n] in rates:
                    rates[processes[n]] = {}
                rates[processes[n]][bins[n]] = float(card_rates[n])
            return rates
        else:
            bins = []
            processes = []
    raise RuntimeError('Rate information not found in "{}"'.format(full_card_file_name))

for mass in masses:
    full_card_file_name = '{}{}/{}'.format(root_dir_prefix, mass, card_file_name)
    rates = ExtractRatesFromDatacard(full_card_file_name)

    for source in sources:
        source_name = source[0]
        source_file_name = '{}{}/{}/{}'.format(root_dir_prefix, mass, ws_dir_name, source[1])
        source_ws_name = source[2].format(mass)

        print '{} -> {}'.format(source_file_name, source_ws_name)
        input_file = TFile(source_file_name, 'READ')
        ws = input_file.Get('w_all')
        for bin,rate in rates[source_name].iteritems():
            var_name = '{}_{}_rate'.format(source_name, bin)
            var = RooRealVar(var_name, var_name, rate)
            getattr(ws, 'import')(var)
        output.WriteTObject(ws, source_ws_name, 'Overwrite')

output.Close()
