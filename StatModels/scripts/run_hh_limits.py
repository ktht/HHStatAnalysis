#!/usr/bin/env python
# Run hh limits.
# This file is part of https://github.com/cms-hh/HHStatAnalysis.

import os
import argparse
import glob
import json
from sets import Set
from HHStatAnalysis.StatModels.run_hh_limits_helpers import *
import libHHStatAnalysisStatModels as HH

parser = argparse.ArgumentParser(description='Run hh limits.',
                  formatter_class = lambda prog: argparse.HelpFormatter(prog, max_help_position=30, width=90))
parser.add_argument('--cfg', required=True, dest='cfg', type=str, metavar='FILE', help="configuration file")
parser.add_argument('--model-desc', required=True, dest='model_desc', type=str, metavar='DESC',
                    help="name of the stat model descriptor in the config")
parser.add_argument('--output', required=True, dest='output_path', type=str, metavar='PATH',
                    help="path were to store limits")
parser.add_argument('--parallel', required=False, dest='n_parallel', type=int, default=8, metavar='N',
                    help="number of parallel jobs")
parser.add_argument('--plotOnly', action="store_true", help="Run only plots.")
parser.add_argument('--collectAndPlot', action="store_true", help="Run only plots.")
parser.add_argument('shapes_file', type=str, nargs='+', help="file with input shapes")

args = parser.parse_args()

limit_json_name = "limits"
limit_json_file = limit_json_name + ".json"
limit_json_pattern = limit_json_name + "_*.json"

if not os.path.exists(args.output_path):
    os.makedirs(args.output_path)

model_desc = HH.LoadDescriptor(args.cfg, args.model_desc)

run_limits = not args.plotOnly and not args.collectAndPlot
collect_limits = run_limits or args.collectAndPlot

if run_limits:
    shapes_file = args.shapes_file[0]
    if len(args.shapes_file) > 1:
        shapes_file = '{}/shapes.root'.format(args.output_path)
        hadd_command = 'hadd -f9 {}'.format(shapes_file)
        for file in args.shapes_file:
            hadd_command += '"{}"'.format(file)
        sh_call(hadd_command, "error while executing hadd for input files")

    sh_call('create_hh_datacards --cfg {} --model-desc {} --shapes {} --output {}'
            .format(args.cfg, args.model_desc, shapes_file, args.output_path),
            "error while executing create_hh_datacards")

limit_type = str(model_desc.limit_type)
if limit_type in Set(['model_independent', 'SM', 'NonResonant_BSM']):
    ch_dir(args.output_path)
    if run_limits:
        sh_call('combineTool.py -M T2W -i */* -o workspace.root --parallel {}'.format(args.n_parallel),
                "error while executing text to workspace")

        sh_call('combineTool.py -M Asymptotic -d */*/workspace.root --there -n .limit --parallel {}'.format(
                args.n_parallel), "error while executing combine")

    if collect_limits:
        sh_call('combineTool.py -M CollectLimits */*/*.limit.* --use-dirs -o {}'.format(limit_json_file),
                "error while collecting limits")

    limits_to_show = "exp"
    if not model_desc.blind:
        limits_to_show += ",obs"

    for input_file in glob.glob(limit_json_pattern):
        output_name = os.path.splitext(input_file)[0]
        y_title = None
        if limit_type == 'SM':
            y_title = "95% CL limit on #sigma / #sigma(SM)"
        else:
            y_title = "95% CL limit on #sigma x BR (pb)"
        sh_call('plotLimits.py {} --output {} --auto-style --y-title \'{}\' --logy --show {}'.format(input_file,
                output_name, y_title, limits_to_show), "error while plotting limits")

elif limit_type == 'MSSM':
    th_model_file_full_path = os.path.abspath(model_desc.th_model_file)
    th_models_path, th_model_file = os.path.split(th_model_file_full_path)
    ch_dir(args.output_path)

    if run_limits or collect_limits:
        grid_dict = { 'opts'  : '--singlePoint 1.0', 'POIs'  : [ 'mA', 'tanb' ], 'grids' : [] }
        grid_dict['grids'].append([
            '{}:{}|{}'.format(model_desc.grid_x.min(), model_desc.grid_x.max(), model_desc.grid_x.step()),
            '{}:{}|{}'.format(model_desc.grid_y.min(), model_desc.grid_y.max(), model_desc.grid_y.step()),
            ''
        ])
        grid_file_name = 'asymptotic_grid.json'
        grid_file = open(grid_file_name, 'w')
        grid_file.write(json.dumps(grid_dict))
        grid_file.close()

    plotLimitGrid = os.environ['CMSSW_BASE'] + '/src/CombineHarvester/CombineTools/scripts/plotLimitGrid.py'
    contours = "exp-2,exp-1,exp0,exp+1,exp+2"
    if not model_desc.blind:
        contours += ",obs"
    workspace_file = "MSSM.root"

    channels = filter(lambda f: os.path.isdir(f), os.listdir('.'))
    for channel in channels:

        if run_limits:
            sh_call('combineTool.py -M T2W -o {} -P {} --PO filePrefix={}/ --PO modelFiles=13TeV,{},1 -i {}'.format(
                    workspace_file, 'CombineHarvester.CombinePdfs.MSSM:MSSM', th_models_path, th_model_file, channel),
                    "error while executing text to workspace")

        work_path = channel + '/work'
        if not os.path.exists(work_path):
            os.makedirs(work_path)
        ch_dir(work_path)

        asymptoticGrid_cmd = 'combineTool.py -M AsymptoticGrid ../../{} -d ../{} --parallel {}'.format(
                             grid_file_name, workspace_file, args.n_parallel)
        if run_limits:
            sh_call(asymptoticGrid_cmd, "error while executing combine")
        if collect_limits:
            sh_call(asymptoticGrid_cmd, "error while collecting jobs")

        output_name = '../../limits_{}_{}'.format(model_desc.limit_type, channel)
        sh_call('{} {} --output {} --contours {}'.format(plotLimitGrid, 'asymptotic_grid.root', output_name, contours),
                "error while plotting limits")

        ch_dir('../..')

else:
    run_failed("limit type '{}' not supported".format(model_desc.limit_type))

run_succeed()
