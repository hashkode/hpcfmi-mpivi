#!/bin/bash
cd "$(dirname "$0")"
_nruns=$1

cd ../build

for i in `seq 1 ${_nruns}`
do
    mpirun -np 2 ./mpi-vi -d
done
