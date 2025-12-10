#!/bin/bash

#SBATCH --mail-user=fbossu@jlab.org
#SBATCH --mail-type=end,fail
#SBATCH --partition=htc
#SBATCH --job-name="tb2025-89-104"
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --time=02:00:00
#SBATCH --mem-per-cpu=4G
#SBATCH --output="log/slurm-%A_%a.out"
#SBATCH --array=89-104
echo $TESTBEAM
export OUTDIR=$TESTBEAM/data2025/run_$SLURM_ARRAY_TASK_ID/feu/
srun --label ./run_feu.sh $SLURM_ARRAY_TASK_ID
#srun --label hostname

