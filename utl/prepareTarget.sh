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
#_project_path='~/projects/hpcmi/mpi-vi/'

cd ..

echo -e "Preparing on host <"${_host}"> with user <"${_git_user}">"

#TODO: remove following section, obsolete due to switch to rsync
#_project_hash=$(md5sum ${_tarname})
#
#_project_hash_target=$(ssh ${_host} "cd ${_project_path}; md5sum ${_tarname}")
#
#if [[ ${_project_hash} != ${_project_hash_target} ]]; then
#  echo -e "Local project md5sum: "${_project_hash}
#  echo -e "Target project md5sum: "${_project_hash_target}
#  echo -e "Copying project to target"
#  scp ${_tarname} ${_host}:${_project_path}
#fi

ssh ${_host} "cd;mkdir -p ${project_path}"
rsync -e "ssh" ./ ${_host}:${project_path} -azr -q --exclude=.idea --delete

if [ ${_runtype} == rebuild ]; then
  ssh ${_host} "cd;cd ${project_path};make init fetchtype=fetch;make preTest;make rebuild"
fi

if [ ${_runtype} == init ]; then
  ssh ${_host} "cd;cd ${project_path};make init fetchtype=fetch;make preTest"
fi
