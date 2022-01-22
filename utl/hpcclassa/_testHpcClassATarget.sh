#!/bin/bash
cd "$(dirname "$0")"
_nruns=$1
_nproca=$2
_nprocb=$3

echo $(hostname)

cd ../../build/
for i in `seq 1 ${_nruns}`
do
    mpirun -np ${_nproca} -hostfile ../utl/hpcclassa/hpcclassa.host --mca btl_tcp_if_include lan0 ./mpi-vi ../automation/jobs/hpcclassa/hpcclassa1.yaml
    mpirun -np ${_nproca} -hostfile ../utl/hpcclassa/hpcclassa.host --mca btl_tcp_if_include lan0 ./mpi-vi ../automation/jobs/hpcclassa/hpcclassa2.yaml
    mpirun -np ${_nproca} -hostfile ../utl/hpcclassa/hpcclassa.host --mca btl_tcp_if_include lan0 ./mpi-vi ../automation/jobs/hpcclassa/hpcclassa3.yaml
done
