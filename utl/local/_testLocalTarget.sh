#!/bin/bash
cd "$(dirname "$0")"
echo $(hostname)

cd ../
configFile_path="$(dirname "$0")/testRunConfig.yaml"
python3 genRunConfig.py -c ${configFile_path}

cd ../build/
_nruns=0
files="../automation/jobs/gen/ds/debug/*.yaml"

for worldSize in 6 ; do
  for file in ${files}; do
    for iRun in `seq 1 ${_nruns}` ; do
      mpirun -np ${worldSize} ./mpi-vi ${file}
    done
  done
done

_nruns=1
files="../automation/jobs/gen/ds/small/*.yaml"

for worldSize in 2 4 6 ; do
  for file in ${files}; do
    for iRun in `seq 1 ${_nruns}` ; do
      mpirun -np ${worldSize} ./mpi-vi ${file}
    done
  done
done

_nruns=1
files="../automation/jobs/gen/ds/normal/*.yaml"

for worldSize in 4 6 ; do
  for file in ${files}; do
    for iRun in `seq 1 ${_nruns}` ; do
      mpirun -np ${worldSize} ./mpi-vi ${file}
    done
  done
done
