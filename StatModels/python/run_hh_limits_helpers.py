# run_hh_limits helpers.
# This file is part of https://github.com/cms-hh/HHStatAnalysis.

import sys
import os
import subprocess
from HHStatAnalysis.StatModels.terminal import colored, bcolors

def run_failed(message):
    print >> sys.stderr, colored("run_hh_limits failed:", bcolors.FAIL), '{}.'.format(message)
    sys.exit(1)

def run_succeed():
    print colored("run_hh_limits successfully finished.", bcolors.OKGREEN)
    sys.exit(0)

def check_result(result, message):
    if result != 0:
        run_failed(message)

def sh_call(cmd, error_message):
    print '>> ', cmd
    result = subprocess.call([cmd], shell=True)
    check_result(result, error_message)

def ch_dir(path):
    try:
        print '>> cd ', path
        os.chdir(path)
    except OSError:
        run_failed("error while changing working directory to '{}'".format(path))
