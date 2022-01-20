#!/bin/bash
cd "$(dirname "$0")"

cd ../build

_nruns=$1

for i in `seq 1 ${_nruns}`
do
    mpirun -np 2 ./mpi-vi -d
done
