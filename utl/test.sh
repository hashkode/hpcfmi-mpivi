#!/bin/bash

### compare md5 check sums from /home/<user> dir and /var/temp/<user>
# copy missing/wrong files

# home_path is the directory with given data, var_path is the directory where the program will later look for data
home_path="/home/$(whoami)/data/"
var_path="/var/tmp/$(whoami)/data"

# check if data directory is already existing
if [ -d "$var_path" ]
then
# data_debug
  tmp="data_debug"
  # check if subdirectory is existing
  if [ -d "$var_path/$tmp" ]
  then
  # calculate and compare checksums
    md5sum $home_path/$tmp/* > checksum_${tmp}_home.chk
    md5sum $var_path/$tmp/* > checksum_${tmp}_temp.chk
  
    if md5sum --status -c checksum_${tmp}_home.chk checksum_${tmp}_temp.chk
    then
      # check if number of files is the same since this is not checked above
      DIFF=$(diff -r $home_path/$tmp $var_path/$tmp)
      if [ -z "$DIFF" ]
        then
          echo "dataset $tmp is okay"
        else
          # dataset is not the same number -> replace
          echo "dataset $tmp is compromised --> copying from home"
          mkdir -p $var_path/$tmp
          cp -R $home_path/$tmp/* $var_path/$tmp
        fi
    else
      # dataset has different checksums -> replace
      echo "dataset $tmp is compromised --> copying from home"
      mkdir -p $var_path/$tmp
      cp -R $home_path/$tmp/* $var_path/$tmp
    fi
  else
    # dataset is not existing -> copying
    echo "dataset $tmp is deleted --> copying from home"
    mkdir -p $var_path/$tmp
    cp -R $home_path/$tmp/* $var_path/$tmp
  fi 
  
# data_small
  tmp="data_small"
  if [ -d "$var_path/$tmp" ]
  then
    md5sum $home_path/$tmp/* > checksum_${tmp}_home.chk
    md5sum $var_path/$tmp/* > checksum_${tmp}_temp.chk
  
    if md5sum --status -c checksum_${tmp}_home.chk checksum_${tmp}_temp.chk
    then
      DIFF=$(diff -r $home_path/$tmp $var_path/$tmp)
      if [ -z "$DIFF" ]
        then
          echo "dataset $tmp is okay"
        else
          echo "dataset $tmp is compromised --> copying from home"
          mkdir -p $var_path/$tmp
          cp -R $home_path/$tmp/* $var_path/$tmp
        fi
    else
      echo "dataset $tmp is compromised --> copying from home"
      mkdir -p $var_path/$tmp
      cp -R $home_path/$tmp/* $var_path/$tmp
    fi
  else
    echo "dataset $tmp is deleted --> copying from home"
    mkdir -p $var_path/$tmp
    cp -R $home_path/$tmp/* $var_path/$tmp
  fi
  
# data_normal
  tmp="data_normal"
  if [ -d "$var_path/$tmp" ]
  then
    md5sum $home_path/$tmp/* > checksum_${tmp}_home.chk
    md5sum $var_path/$tmp/* > checksum_${tmp}_temp.chk
  
    if md5sum --status -c checksum_${tmp}_home.chk checksum_${tmp}_temp.chk
    then
      DIFF=$(diff -r $home_path/$tmp $var_path/$tmp)
      if [ -z "$DIFF" ]
        then
          echo "dataset $tmp is okay"
        else
          echo "dataset $tmp is compromised --> copying from home"
          mkdir -p $var_path/$tmp
          cp -R $home_path/$tmp/* $var_path/$tmp
        fi
    else
      echo "dataset $tmp is compromised --> copying from home"
      mkdir -p $var_path/$tmp
      cp -R $home_path/$tmp/* $var_path/$tmp
    fi
  else
    echo "dataset $tmp is deleted --> copying from home"
    mkdir -p $var_path/$tmp
    cp -R $home_path/$tmp/* $var_path/$tmp
  fi 
# data directory not existing -> copy complete dataset
else
  echo "no dataset in /var/temp/ --> copying files ..."
  mkdir -p $var_path
  cp -R $home_path/* $var_path
  echo "copying finished"
fi

# remove checksum files -> not needed anymore
rm checksum*

### run binary
cd "$(dirname "$0")"
cd ../build
mpirun -np 6 ./mpi-vi "../automation/jobs/test.yaml"

# me="$(whoami)" for username
