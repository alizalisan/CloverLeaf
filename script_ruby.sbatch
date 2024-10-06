#!/bin/bash
#SBATCH -N 1
#SBATCH -J Cloverleaf
#SBATCH -t 01:00:00
#SBATCH -p pdebug

PERFFLOW_OPTIONS="log-event=compact" /usr/bin/time srun -N48 --ntasks-per-node=1 -o test.out -e test.err /p/vast1/cloudgap/CloverLeaf/build_ruby_pfa/omp-cloverleaf --in /p/vast1/cloudgap/CloverLeaf/InputDecks/ballsoffury_overhead_ruby.in