#!/bin/bash
cd "$(dirname "$0")"
_nruns=$1
_nproca=$2
_nprocb=$3

cd ../../build

echo $(hostname)

for i in `seq 1 ${_nruns}`
do
    mpirun -np ${_nproca} -hostfile ../utl/nuc/nuc.host ./mpi-vi ../automation/jobs/nuc/default.yaml
done

cd ../
make postTest
