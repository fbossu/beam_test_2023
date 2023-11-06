#!/bin/bash

detName=asaFEU4
FEUname=FEU4

# Loop through all the input files in the input directory
for bancoPath in ../../banco/rec_POS??.root; do
    bancoFile=$(basename "$bancoPath" .root)
    MMFile="${bancoFile}_${FEUname}.root"
    ./alignment $detName $bancoPath ../../position/${MMFile}
done
