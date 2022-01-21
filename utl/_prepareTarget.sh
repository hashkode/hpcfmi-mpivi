#!/bin/bash
cd "$(dirname "$0")"
_host=$1
_runtype=$2

_git_user=$(git config --get user.email)

_target_user=$(ssh ${_host} 'whoami')

home_path="/home/${_target_user}/data/"
var_path="/var/tmp/${_target_user}/data"
results_path="/var/tmp/${_target_user}/results"

project_path="/home/${_target_user}/projects/hpcmi/mpi-vi/"

cd ..

echo -e "Preparing on host <"${_host}"> with user <"${_git_user}">"

ssh ${_host} "cd;mkdir -p ${project_path}"
rsync -e "ssh" ./ ${_host}:${project_path} -azr -q --exclude=.idea --delete

if [ ${_runtype} == build ]; then
  ssh ${_host} "cd;cd ${project_path};make init fetchtype=fetch;make preTest;make build"
fi

if [ ${_runtype} == init ]; then
  ssh ${_host} "cd;cd ${project_path};make init fetchtype=fetch;make preTest"
fi
