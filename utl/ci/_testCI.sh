#!/bin/bash
cd "$(dirname "$0")"

cd ../../build

mpirun ./mpi-vi ../automation/jobs/ci/scheme1.yaml
mpirun ./mpi-vi ../automation/jobs/ci/scheme2.yaml
mpirun ./mpi-vi ../automation/jobs/ci/scheme3.yaml
