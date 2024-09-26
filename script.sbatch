#!/bin/bash
#SBATCH -N 1
#SBATCH -J Cloverleaf
#SBATCH -t 02:00:00

/usr/bin/time srun -N1 --ntasks-per-node=48 /p/vast1/cloudgap/CloverLeaf/build_corona_pfa/omp-cloverleaf --in /p/vast1/cloudgap/CloverLeaf/InputDecks/ballsoffury.in --out ballsoffury.out