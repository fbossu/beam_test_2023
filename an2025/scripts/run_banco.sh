#!/bin/bash

NEV=$2
CODE=$TESTBEAM/code
for a in $1/banco_data/*root; 
do 
  echo $a
#  n=$( echo $a | cut -d- -f3) 
  n=${a:$((${#a}-14)):9}.root
  $CODE/banco/decode_banco $a $TESTBEAM/noise/multinoiseScan_251108_100636-B0-$n 
  mv -v fdec.root $n 
done

$CODE/banco/reco_banco -d $CODE/banco/geometrySPS2025 -r -m 0.3 ladder1*root -n $NEV

