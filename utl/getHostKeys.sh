#!/bin/bash
cd "$(dirname "$0")"

ssh-keyscan -t rsa rsync.hidrive.strato.com >> ~/.ssh/known_hosts

ssh-keyscan -t rsa hpc01 >> ~/.ssh/known_hosts
ssh-keyscan -t rsa hpc02 >> ~/.ssh/known_hosts
ssh-keyscan -t rsa hpc03 >> ~/.ssh/known_hosts
ssh-keyscan -t rsa hpc04 >> ~/.ssh/known_hosts
ssh-keyscan -t rsa hpc05 >> ~/.ssh/known_hosts
ssh-keyscan -t rsa hpc06 >> ~/.ssh/known_hosts
ssh-keyscan -t rsa hpc07 >> ~/.ssh/known_hosts
ssh-keyscan -t rsa hpc08 >> ~/.ssh/known_hosts
ssh-keyscan -t rsa hpc09 >> ~/.ssh/known_hosts
ssh-keyscan -t rsa hpc10 >> ~/.ssh/known_hosts
ssh-keyscan -t rsa hpc11 >> ~/.ssh/known_hosts
ssh-keyscan -t rsa hpc12 >> ~/.ssh/known_hosts
ssh-keyscan -t rsa hpc13 >> ~/.ssh/known_hosts
ssh-keyscan -t rsa hpc14 >> ~/.ssh/known_hosts
ssh-keyscan -t rsa hpc15 >> ~/.ssh/known_hosts

ssh-keyscan -t rsa marco >> ~/.ssh/known_hosts
ssh-keyscan -t rsa polo >> ~/.ssh/known_hosts
