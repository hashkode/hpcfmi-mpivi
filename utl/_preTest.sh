#!/bin/bash
cd "$(dirname "$0")"

results_path="/var/tmp/$(whoami)/results/"

### sync results directory from server
echo ">> fetch results"
mkdir -p ${results_path}
rsync -e "ssh -i sshkey -o 'StrictHostKeyChecking no'" hpcfmi@rsync.hidrive.strato.com:users/hpcfmi/results/ $results_path -azrc -q
