#!/bin/env python

import os
import sys
import subprocess
import json


# name of the detector under studye
detname = sys.argv[ 1 ]
detname = detname.strip()


# open run_config.json file
fname = sys.argv[2 ]
print( detname, fname )
fin = open(fname)
try:
    jd = json.load(fin) 
except:
    print("error: file name not given or invalid")
    exit()

# check if the detector is among the included ones
indets = jd['included_detectors']
if detname not in indets:
    print( "error: ", detname, " is not among the detectors of this run");
    exit()

run = jd["run_name"]
outdir = jd["data_out_dir"]
subruns = jd["sub_runs"]

TB  = os.environ["TESTBEAM"]
BTC = os.environ["BEAMTESTCODE"]
print(BTC)
cd = os.getcwd()
for sr in subruns:
    srn = sr["sub_run_name"]
    print( srn )
    bancofile = cd+"/../banco/" + srn + "/fout.root"
    
    # check that the banco file exist
    if not os.path.isfile(bancofile):
        print( "missing banco file for subrun " + srn )
        continue
    mmfile = cd + "/reco_"+ detname +"_"+srn+".root"

    # check that the mm file exist
    if not os.path.isfile(mmfile):
        print( "missing mm file for subrun " + srn )
        continue

    try:
        os.mkdir(srn)
    except FileExistsError:
        print( "Warning: dir " + srn + " alreayd exists")

    os.chdir( srn )
    subprocess.run( [
        TB + "code/an2025/an",
        "-d" + detname,
        "-m"+mmfile ,
        "-b"+bancofile,
        "-A" + TB+'/code/an2025/alignFiles_89_104',
        "-t10." 
        ] + sys.argv[3:] )
    os.chdir( cd )
