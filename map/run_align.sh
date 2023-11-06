#!/bin/bash

detName=stripFEU1
FEUname=FEU1_strip

# Loop through all the input files in the input directory
for bancoPath in ../../banco/rec_POS??.root; do
    bancoFile=$(basename "$bancoPath" .root)
    MMFile="${bancoFile}_${FEUname}.root"
    MMPath="../../position/${MMFile}"
    echo "Aligning $MMPath with respect to $bancoPath"
    ./alignment $detName $bancoPath $MMPath
done
