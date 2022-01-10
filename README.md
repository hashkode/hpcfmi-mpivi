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
