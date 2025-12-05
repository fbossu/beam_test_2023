# Beam test 2023

All the decoding and reconstruction software uses the logic that at each step the output file is recreated in the same directory than the code with a standard name. Then a script can be used to move and rename the final files to be kept.

## Decoding fdf

```
cd decode
make
./decode filename.fdf
```
Outputs decoded file ftest.root, where each event has sample,channel,amplitude vectors of the same size. 

## Decoding Non-Zero Suppressed fdf

```
cd decode
make
./decode data_file.fdf out_data.root
./decode pedestal_file.fdf out_pedestal.root
./subtract_pedestals out_data.root out_pedestal.root out_ped_sub.root
```
Given a data fdf (data_file.fdf) and pedestal fdf (pedestal_file.fdf), decode both. Then subtract pedestals (no common noise subtraction currently impelemented) and output a pedestal subtracted root file.

## FEU reconstruction

### NEW version
The new version of the reconstruction exploits the `json` configuration file used for the data taking
```
cd reco
make
./reco_det -j PATH/TO/JSON -d DETNAME
```
Run it with `-h` to get the full list of options.\
*Attention* be sure that the paths in the json files exists. ~Suggestion~ You can make a copy of the json file and then change the internal paths to the real location of the data.

### old version
```
cd reco
make
./reco file.root FEUNb
```
Takes a decoded file and a FEU number.\
Use FEUNb=5 to only make hits no need for any mapping info.\
FEU 1 to 4 were connected to specific detectors in the beam test. The code knows which detector is connected to which FEU and how the connectors were plugged. It then pulls the maping for that detector and build hits&clusters.\

[detector hits and cluster definition](reco/definitions.h)

## Decoding Alpide output

Takes the Alpide output root files for one ladder and a noise file for that ladder to find the noisy pixels.
```
cd banco
make
./decode_banco ladder.root noise.root
```
Outputs fdec.root where each event has a hits and a clusters vector filled with all the reconstructed banco::hit and banco::cluster object reconstructed in that event.\
[banco hits, cluster and track definition](banco/definition_banco.h)

## Tracks reconstruction

Takes the decoded files of all 4 ladders for that runs
```
./reco_banco ladder*.root
```
Outputs a root file with for each event a vector with all the banco::track objects.
Many options to input an alignment file with the offsets of each ladder, and a global rotation file. Also an option to compute residues and add the plots to the output root file.
