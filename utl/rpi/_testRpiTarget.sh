#!/bin/bash
cd "$(dirname "$0")"
_nruns=$1
_nproca=$2
_nprocb=$3

cd ../../build

echo $(hostname)

for i in `seq 1 ${_nruns}`
do
    mpirun -np ${_nproca} -hostfile ../utl/rpi/rpi.host -mca btl_tcp_if_include eth0 ./mpi-vi ../automation/jobs/rpi/default.yaml
done

cd ../
make postTest
