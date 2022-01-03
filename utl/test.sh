#!/bin/bash

# compare md5 check sums from /home/<user> dir and /var/temp/<user>
# copy missing/wrong files

home_path="/home/$(whoami)/data/"
var_path="/var/tmp/$(whoami)/data"

if [ -d "$var_path" ]
then
# data_debug
  tmp="data_debug"
  if [ -d "$var_path/$tmp" ]
  then
    md5sum $home_path/$tmp/* > checksum_${tmp}_home.chk
    md5sum $var_path/$tmp/* > checksum_${tmp}_temp.chk
  
    if md5sum --status -c checksum_${tmp}_home.chk checksum_${tmp}_temp.chk
    then
      DIFF=$(diff $home_path/$tmp $var_path/$tmp) 
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
  
# data_small
  tmp="data_small"
  if [ -d "$var_path/$tmp" ]
  then
    md5sum $home_path/$tmp/* > checksum_${tmp}_home.chk
    md5sum $var_path/$tmp/* > checksum_${tmp}_temp.chk
  
    if md5sum --status -c checksum_${tmp}_home.chk checksum_${tmp}_temp.chk
    then
      DIFF=$(diff $home_path/$tmp $var_path/$tmp) 
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
      DIFF=$(diff $home_path/$tmp $var_path/$tmp) 
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
  
else
  echo "no dataset in /var/temp/ --> copying files ..."
  #mkdir /var/tmp/$(whoami)
  mkdir -p $var_path
  cp -R $home_path/* $var_path
  echo "copying finished"
fi

rm checksum*

cd "$(dirname "$0")"
cd ../build
./mpi-vi

# me="$(whoami)" for username
