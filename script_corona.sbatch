#!/bin/bash
#SBATCH -N 1
#SBATCH -J Cloverleaf
#SBATCH -t 01:00:00

export HIP_VISIBLE_DEVICES=0,1,2,3,4,5,6,7
PERFFLOW_OPTIONS="log-event=compact" /usr/bin/time srun -N1 --ntasks-per-node=1 --exclusive -o test.out -e test.err /p/vast1/cloudgap/CloverLeaf/build_corona_pfa/hip-cloverleaf --in /p/vast1/cloudgap/CloverLeaf/InputDecks/ballsoffury_corona.in