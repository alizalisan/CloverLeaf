#!/bin/bash
#SBATCH -N 1
#SBATCH -J Cloverleaf
#SBATCH -t 04:00:00

/usr/bin/time srun -N1 --ntasks-per-node=48 /p/vast1/cloudgap/CloverLeaf/build/omp-cloverleaf --in /p/vast1/cloudgap/CloverLeaf/InputDecks/ballsoffurry.in --out ballsoffurry.out