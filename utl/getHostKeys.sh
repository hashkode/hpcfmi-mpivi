#!/bin/bash
cd "$(dirname "$0")"

# permission adjustment for sshkey
chmod 400 sshkey

ssh-keyscan -t rsa rsync.hidrive.strato.com >> ~/.ssh/known_hosts
rsync --dry-run -e "ssh -i sshkey" hpcfmi@rsync.hidrive.strato.com:users/hpcfmi/results/ ~ -azr -q

ssh hpc01 "hostname"
ssh hpc02 "hostname"
ssh hpc03 "hostname"
ssh hpc04 "hostname"
ssh hpc05 "hostname"
ssh hpc06 "hostname"
ssh hpc07 "hostname"
ssh hpc08 "hostname"
ssh hpc09 "hostname"
ssh hpc10 "hostname"
ssh hpc11 "hostname"
ssh hpc12 "hostname"
ssh hpc13 "hostname"
ssh hpc14 "hostname"
ssh hcp15 "hostname"

# get nuc keys
# ssh marco "hostname"
# ssh polo "hostname"
