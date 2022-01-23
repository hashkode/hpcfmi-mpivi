#!/bin/bash
cd "$(dirname "$0")"
echo $(hostname)

_hostfile=../utl/hpcclassb/hpcclassb.host

cd ../
configFile_path="$(dirname "$0")/testRunConfig.yaml"
python3 genRunConfig.py -c ${configFile_path}

cd ../build/
_nruns=0
files="../automation/jobs/gen/ds/debug/*.yaml"

for ppn in 2 ; do
  for file in ${files}; do
    for iRun in `seq 1 ${_nruns}` ; do
      mpirun --map-by ppr:${ppn}:node -hostfile ${_hostfile} -mca btl_tcp_if_include lan0 ./mpi-vi ${file}
    done
  done
done

_nruns=1
files="../automation/jobs/gen/ds/small/*.yaml"

for ppn in 1 2 3 ; do
  for file in ${files}; do
    for iRun in `seq 1 ${_nruns}` ; do
      mpirun --map-by ppr:${ppn}:node -hostfile ${_hostfile} -mca btl_tcp_if_include lan0 ./mpi-vi ${file}
    done
  done
done

_nruns=0
files="../automation/jobs/gen/ds/normal/*.yaml"

for ppn in 2 3 ; do
  for file in ${files}; do
    for iRun in `seq 1 ${_nruns}` ; do
      mpirun --map-by ppr:${ppn}:node -hostfile ${_hostfile} -mca btl_tcp_if_include lan0 ./mpi-vi ${file}
    done
  done
done
