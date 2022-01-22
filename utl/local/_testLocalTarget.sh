#!/bin/bash
cd "$(dirname "$0")"
_nruns=$1
_nproca=$2

echo $(hostname)

cd ../../build/
for i in `seq 1 ${_nruns}`
do
    mpirun -np ${_nproca} ./mpi-vi ../automation/jobs/local/local1.yaml
    mpirun -np ${_nproca} ./mpi-vi ../automation/jobs/local/local2.yaml
    mpirun -np ${_nproca} ./mpi-vi ../automation/jobs/local/local3.yaml
done
