#!/bin/bash
cd "$(dirname "$0")"
_nruns=$1
_nproc=$2

cd ../../build

echo $(hostname)

for i in `seq 1 ${_nruns}`
do
    mpirun -np ${_nproc} -hostfile ../utl/nuc/nuc.host ./mpi-vi ../automation/jobs/nuc/default.yaml
done

cd ../
make postTest
