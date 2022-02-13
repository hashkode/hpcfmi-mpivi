#!/bin/bash
cd "$(dirname "$0")"

results_path="/var/tmp/$(whoami)/results"

# TODO: analyse and repair rsync setup
### sync results directory to server
#rsync -e "ssh -i sshkey -o 'StrictHostKeyChecking no'" $results_path hpcfmi@rsync.hidrive.strato.com:users/hpcfmi/ -azrc -q
