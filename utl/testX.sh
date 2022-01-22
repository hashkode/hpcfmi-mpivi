#!/bin/bash
cd "$(dirname "$0")"
_nruns=$1

cd ../build

for i in `seq 1 ${_nruns}`
do
    mpirun ./mpi-vi ../automation/jobs/default.yaml
    mpirun ./mpi-vi ../automation/jobs/scheme2.yaml
    mpirun ./mpi-vi ../automation/jobs/scheme3.yaml
done
