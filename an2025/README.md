# Analysis repository for the 2025 test beam

The general idea is to have analysis code as separate **plugins**. \
`analysis.cpp` is the main program. You can additional plugins for specific analyses.


```bash
make
./an -d DETNAME -m MMfile.root -b Bancofile.root
```

Options
```
Usage: an -d DETNAME -m MMfile.root -b Bancofile.root
Options: 
 -d [string] detector name
 -m [string] MM root file
 -b [string] banco root file
 -B [float] banco y position
 -M [string] mapping file
 -A [string] alignment direcotry
 -n [int] max numer of events
 -s [int] skip events
```

## Adding a plugin

Any plugin must inherit from `anplug.h`.

It has to have thee main methodes: init, run, end

Then, in `analysis.cpp` the new plugin can be added to the map of plugins.


## Analysis workflow

### Setup the enviroment

On **feyman** one can use the software provided by LCG via /cvmfs.

_Suggestion_ Create a `setup.sh` that can be sourced or put these lines in your .bashrc (or equivalent)
```bash
source /cvmfs/sft.cern.ch/lcg/views/LCG_101/x86_64-centos8-gcc11-opt/setup.sh

export BEAMTESTCODE=$TESTBEAM/code
```

### Prepare the runs

Data are stored on **feynman** here
```bash
/feynman/scratch/dphn/lsn/dn277127/beam_sps_25/Runs
```

First of all, in your work directory, you should copy the `run_config.json` files and you should change the internal paths to point to the appropriate directory.
You can use the following command
```bash
for a in {48..54}; do mkdir -p run_$a ; cat /feynman/scratch/dphn/lsn/dn277127/beam_sps_25/Runs/run_$a/run_config.json | sed -e 's/\/mnt\/data\/beam_sps_25\/Run/\/feynman\/scratch\/dphn\/lsn\/dn277127\/beam_sps_25\/Runs/g' > run_$a/run_config.json ; done
```

### Banco reconstruction

Banco data must be decoded and then a simple reconstruction creates tracks.

Use `get_banco_files_per_run.py` that you find in the scripts folder. As input you must give the `run_config.json` file.

### rd5 reconstruction

The main program is `reco_det`. Follow the helper -h to get the full options.

You can use the `run_feu.sh` and `job.sh` to run the rd5 reconstruction in batch

### run the analysis program

For easy use and automatic running over the several subruns, you can use the `analysis.py` python script that you can find in the `scripts` folder.

Run it like
```bash
$BEAMTESTCODE/an2025/scripts/analysis.py [detector name] [...run_config.json] [options]
```
