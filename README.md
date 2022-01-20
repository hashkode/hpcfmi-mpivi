Hauptprojekt
============

# Measurement objectives

Possible measurements:
- Prio A:
  - execution time (total, per block)
  - iterations until convergence
- Prio B:
  - memory usage (RAM)
- Prio C:
  - I/O consumption

Possible variatiation points:
- Prio A:
  - MPI schemes (minimum 2)
  - MPI synchronization intervall (cycles)
  - MPI processor count
- Prio B:
  - computing hardware (HPC class 1, HPC class 2, HPC class mixed, RPi cluster, NUC cluster)
  - asynchronous vs. synchronous VI with OpenMP
- Prio C:
  - J split algorithm
  - baremetal vs. cluster/kubernetes

# Procedure

**init**
Downloads the dataset and prepares it for cpp.
**test**
Checks integrity of data in /var/tmp/user/ and the downloaded data.

In /automation/jobs/ lay yaml files with the desired parameters/configuration.

# Makefile

- **clean**:
  - Cleans repository and removes build files.
- **compile**:  --> here and at **test** specify which scheme(s)? default all schemes?
  - **clean**
  - doxygen
  - builds project
- **init**:
  - updates submodules
  - checks and installs neccessary packages
  - downloads data-set from strato server
  - prepares data-set according to _preparedata.py_
- **test**:
  - **compile**
  - checks and updates data in local directory according to downloaded one from **init**
  - runs executable
- **report**:
  - makes latex report
- **pack**:
  - **clean**
  - packs project
- **unpack**:
  - unpacks project
- **send**:
  - send packed project to specified host
- **all**:
  - tbd
