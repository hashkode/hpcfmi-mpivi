#!/bin/bash
cd "$(dirname "$0")"
_nruns=$1
_nproc=$2

cd ../../build

echo $(hostname)

for i in `seq 1 ${_nruns}`
do
    mpirun -np ${_nproc} -hostfile ../utl/hpcclassa/hpcclassa.host --mca btl_tcp_if_include lan0 ./mpi-vi ../automation/jobs/hpcclassa/default.yaml
done

cd ../
make postTest
