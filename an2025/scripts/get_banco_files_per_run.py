#!/bin/env python

import os
import sys
import subprocess
import json


fname = sys.argv[-1]
fin = open(fname)

try:
    jd = json.load(fin) 
except:
    print("error: file name not given or invalid")
    exit()

run = jd["run_name"]
outdir = jd["data_out_dir"]
subruns = jd["sub_runs"]

TB = os.environ["TESTBEAM"]
print(TB)
cd = os.getcwd()
for sr in subruns:
    srn = sr["sub_run_name"]
    try:
        os.mkdir(srn)
    except FileExistsError:
        print( "Warning: dir " + srn + " alreayd exists")

    os.chdir( srn )
    print( os.getcwd() )

    subprocess.run( [
        TB+"code/an2025/scripts/run_banco.sh",
        outdir+"/"+run+"/"+srn,
        "-1"
    ] )
    os.chdir( cd )

    
