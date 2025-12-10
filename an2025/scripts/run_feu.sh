#!/bin/bash
source $TESTBEAM/setup.sh
BEAMTEST=$TESTBEAM/code
RUN=$1
DETS=$TESTBEAM/data2025/dets_89_104

DATADIR=$TESTBEAM/data2025/run_$RUN

echo $RUN 
echo $DATADIR
echo $OUTDIR

mkdir -v -p $SCRATCH/feu/$RUN && cd $_
pwd

while read DET
do
$BEAMTEST/reco/reco_det -j $DATADIR/run_config.json -d $DET
done < $DETS

ls

# move results to output directory
##################################

mkdir -p $OUTDIR
mv -v *.root $OUTDIR
#mkdir -p $OUTDIR/log
#mv -v *.log $OUTDIR/log
