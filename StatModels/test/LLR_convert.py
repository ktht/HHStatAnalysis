#!/usr/bin/env python
# Convert LLR bbtautau workspaces into histograms with the common naming convention.
# This file is part of https://github.com/cms-hh/HHStatAnalysis.

import os
import argparse
import glob
from ROOT import TFile, RooFit

parser = argparse.ArgumentParser(description='Convert LLR workspaces into the common format.',
                  formatter_class = lambda prog: argparse.HelpFormatter(prog, max_help_position=30, width=90))
parser.add_argument('--input', required=True, dest='input_path', type=str, metavar='INPUT_PATH',
                    help="configuration file")
parser.add_argument('--output', required=True, dest='output', type=str, metavar='OUTPUT_FILE',
                    help="path were to store limits")
args = parser.parse_args()

output = TFile(args.output, 'RECREATE')
os.chdir(args.input_path)

file_name_pattern = '{}/hh_{}_C{}_LRadion{}_13TeV.input.root'
era = "13TeV"
channels = { 1: 'eTau', 2: 'muTau', 3: 'tauTau' }
categories = { 1: 'res1b', 2: 'res2b', 3: 'boosted' }
masses = [ 250, 260, 270, 280, 300, 320, 340, 400, 450, 500, 550, 600, 650, 700, 750, 800, 900 ]
scales = { 'scale_tau' : 'scale_tau', 'qcd_btag_relax' : 'qcd_RlxToTight_{}_{}', 'topPt' : 'pttopreweight' }
scale_variations = [ 'Up', 'Down' ]
bins = {
    'ggRadion'  : [ [ 'ggHH_hbbhtt' ], [ 'scale_tau' ], True, 10 ],
    'data_obs'  : [ [ 'data_obs' ], [], False, 1 ],
    'TT'        : [ [ 'bkg_TT' ], [ 'scale_tau', 'topPt' ], False, 1 ],
    'DY_0b'     : [ [ 'bkg_DY0b' ], [ 'scale_tau' ], False, 1 ],
    'DY_1b'     : [ [ 'bkg_DY1b' ], [ 'scale_tau' ], False, 1 ],
    'DY_2b'     : [ [ 'bkg_DY2b' ], [ 'scale_tau' ], False, 1 ],
    'QCD'       : [ [ 'bkg_QCD' ], [ 'qcd_btag_relax' ], False, 1 ],
    'W'         : [ [ 'bkg_WJet' ], [ 'scale_tau' ], False, 1 ],
    'tW'        : [ [ 'bkg_TWtop', 'bkg_TWantitop' ], [ 'scale_tau' ], False, 1 ],
    'VV'        : [ [ 'bkg_WWToLNuQQ', 'bkg_WZTo1L1Nu2Q', 'bkg_WZTo1L3Nu', 'bkg_WZTo2L2Q', 'bkg_ZZTo2L2Q' ],
                    [ 'scale_tau' ], False, 1 ]
}

def LoadHistogram(ws, mHH, hist_name, file_name, raise_exception = True):
    data_hist = ws.data(hist_name)
    if data_hist == None:
        if not raise_exception:
            return None
        raise RuntimeError('Histogram "{}" not found in "{}".'.format(hist_name, file_name))
    return data_hist.createHistogram(hist_name, mHH)

def CreateEmptyHistogram(ref_hist, channel, category):
    hist =  ref_hist.Clone('default_hist_{}_{}'.format(channel, category))
    small_yield = 1e-20
    for n in range(0, hist.GetNbinsX() + 2):
        hist.SetBinContent(n, 0)
        hist.SetBinError(n, 0)
    n = hist.GetNbinsX() / 2
    hist.SetBinContent(n, small_yield)
    hist.SetBinError(n, small_yield)
    return hist

def MakeScaleHistName(hist_name, scale_name, scale_variation, channel, category):
    if scale_name == 'qcd_btag_relax':
        return '{}_CMS_shape_{}_hh_ttbb_{}_{}_{}{}'.format(hist_name, scale_name, channel, category, era,
                                                           scale_variation)
    return '{}_CMS_shape_{}_{}{}'.format(hist_name, scale_name, era, scale_variation)

for channel_idx,channel in channels.iteritems():
    channel_dir = channel[0].upper() + channel[1:]
    print '{} -> {}'.format(channel_dir, channel)
    for category_idx,category in categories.iteritems():
        output_dir_name = '{}_{}'.format(channel, category)
        print '\tprocessing {}'.format(category)
        output_dir = output.mkdir(output_dir_name)
        bkg_obs_extracted = False
        default_hist = None
        for mass in masses:
            file_name = file_name_pattern.format(channel_dir, channel_idx, category_idx, mass)
            print '\t\t mH={}GeV, {} -> {}'.format(mass, file_name, output_dir_name)
            input_file = TFile(file_name, 'OPEN')
            ws = input_file.Get('w')
            mHH = ws.var('HHKin_mass_raw')
            if mHH == None:
                raise RuntimeError('Variable "HHKin_mass_raw" not found.')

            for bin,bin_desc in bins.iteritems():
                if bkg_obs_extracted and not bin_desc[2]: continue
                hist = None
                scale_hists = {}
                hist_name = bin
                if bin_desc[2]:
                    hist_name = '{}_M{}'.format(bin, mass)
                print '\t\t\t creating {}'.format(hist_name)
                for data_hist_name in bin_desc[0]:
                    print '\t\t\t\t {}'.format(data_hist_name)
                    new_hist = LoadHistogram(ws, mHH, data_hist_name, file_name, False)
                    if new_hist == None: continue
                    if hist == None:
                        hist = new_hist
                    else:
                        hist.Add(new_hist)
                    for scale_name in bin_desc[1]:
                        original_scale_name = scales[scale_name]
                        if original_scale_name.find('{}') != -1:
                            original_scale_name = original_scale_name.format(channel_idx, category_idx)
                        for scale_variation in scale_variations:
                            scale_data_hist_name = '{}_CMS_HHbbtt_{}{}'.format(data_hist_name, original_scale_name,
                                                                               scale_variation)
                            print '\t\t\t\t {}'.format(scale_data_hist_name)
                            scale_hist_name = MakeScaleHistName(hist_name, scale_name, scale_variation, channel,
                                                                category)
                            new_scale_hist = LoadHistogram(ws, mHH, scale_data_hist_name, file_name)
                            if scale_hist_name in scale_hists:
                                scale_hists[scale_hist_name].Add(new_scale_hist)
                            else:
                                scale_hists[scale_hist_name] = new_scale_hist

                if hist == None:
                    if default_hist == None:
                        raise RuntimeError('Can not create empty histogram for {} {}'.format(channel, category))
                    hist = default_hist
                if default_hist == None:
                    default_hist = CreateEmptyHistogram(hist, channel, category)
                hist.Scale(bin_desc[3])
                output_dir.WriteTObject(hist, hist_name, 'Overwrite')

                for scale_name in bin_desc[1]:
                    for scale_variation in scale_variations:
                        scale_hist_name = MakeScaleHistName(hist_name, scale_name, scale_variation, channel, category)
                        scale_hist = None
                        if scale_hist_name in scale_hists:
                            scale_hist = scale_hists[scale_hist_name]
                        else:
                            scale_hist = default_hist
                        scale_hist.Scale(bin_desc[3])
                        output_dir.WriteTObject(scale_hist, scale_hist_name, 'Overwrite')

            input_file.Close()
            bkg_obs_extracted = True

output.Close()
