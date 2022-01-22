#!/bin/bash
cd "$(dirname "$0")"
_nruns=$1
_nproca=$2
_nprocb=$3

echo $(hostname)

cd ../../build/
for i in `seq 1 ${_nruns}`
do
    mpirun -np ${_nproca} -hostfile ../utl/hpcclassb/hpcclassb.host --mca btl_tcp_if_include lan0 ./mpi-vi ../automation/jobs/hpcclassb/hpcclassb1.yaml
    mpirun -np ${_nproca} -hostfile ../utl/hpcclassb/hpcclassb.host --mca btl_tcp_if_include lan0 ./mpi-vi ../automation/jobs/hpcclassb/hpcclassb2.yaml
    mpirun -np ${_nproca} -hostfile ../utl/hpcclassb/hpcclassb.host --mca btl_tcp_if_include lan0 ./mpi-vi ../automation/jobs/hpcclassb/hpcclassb3.yaml
done
