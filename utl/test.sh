#!/bin/bash

# compare md5 check sums from /home/<user> dir and /var/temp/<user>
# copy missing/wrong files

if [ -d "/var/temp/$(whoami)" ]
then
  md5sum /home/$(whoami)/data/data_debug/* > checksum_debug_home.chk
  md5sum /var/temp/$(whoami)/data/data_debug/* > checksum_debug_temp.chk
  
  if [md5sum --status -c checksum_debug_home.chk checksum_debug_temp.chk]
  then
  else
    echo "dataset data_debug is compromised --> copying from home"
    cp -R /home/$(whoami)/data_debug/* /var/temp/$(whoami)/data/data_debug
  fi
  
  md5sum /home/$(whoami)/data/data_small/* > checksum_small_home.chk
  md5sum /var/temp/$(whoami)/data/data_small/* > checksum_small_temp.chk
  
  if [md5sum --status -c checksum_small_home.chk checksum_small_temp.chk]
  then
  else
    echo "dataset data_small is compromised --> copying from home"
    cp -R /home/$(whoami)/data_debug/* /var/temp/$(whoami)/data/data_debug
  fi
  
  md5sum /home/$(whoami)/data/data_normal/* > checksum_normal_home.chk
  md5sum /var/temp/$(whoami)/data/data_normal/* > checksum_normal_temp.chk
  
  if [md5sum --status -c checksum_normal_home.chk checksum_normal_temp.chk]
  then
  else
    echo "dataset data_normal is compromised --> copying from home"
    cp -R /home/$(whoami)/data_debug/* /var/temp/$(whoami)/data/data_debug
  fi
  
else
  echo "no dataset in /var/temp/ --> copying files ..."
  mkdir /var/temp/$(whoami)/data
  cp -R /home/$(whoami)/data/* /var/temp/$(whoami)/data
fi

cd "$(dirname "$0")"

cd ../build
./mpi-vi

# me="$(whoami)" for username
