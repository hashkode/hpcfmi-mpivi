#include "FirstSchema.h"

void FirstSchema::AsynchronousValueIteration(Eigen::Map<Eigen::VectorXf> &j, Eigen::Map<SparseMatrixType> &p,
                                                Eigen::Map<Eigen::VectorXi> pi, float alpha, int maxF, int nStars, int maxU,
                                                float epsThreshold,int maxIteration, int commPeriode)
 {
     // init open mpi world and rank
     int world_size, world_rank;
     MPI_Comm_size(MPI_COMM_WORLD, &world_size); // Number of processes
     MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // Rank of this process


     // Split up states for each rank
     const int firstState = (j.size() / world_size) * world_rank;
     const int lastState=(j.size()/ world_size) * (world_rank + 1);


     // set number of states per process and displacements of subvectors
     std::vector<int> nStatesPerProcess,displs;    // Number of states and Displacement of sub vectors for each process
     for(int i=0; i < world_size; ++i)
     {
         if(i+1 < world_size) nStatesPerProcess.push_back((j.size() / world_size));
         else nStatesPerProcess.push_back((j.size() / world_size) + j.size() % world_size);

         if(i == 0) displs.push_back(0);
         else displs.push_back(displs[i-1] + nStatesPerProcess[i-1]);
     }

     // Keeps track of the change in J vector
     float error = 0;
     for(unsigned int t=0; t < maxIteration; ++t)
     {
         // Compute one value iteration step for a range of states
         float epsGlobal = Backend::ValueIterationStep() // TODO : set iteration parameter

         // Store value of biggest change that appeared while updating J
         if(epsGlobal > error) {
             error = epsGlobal;
         };

         // Exchange results every comm_period cycles
         if(t % commPeriode== 0)
         {
             // Merge local updates together
             float* J_raw = j.data();
             MPI_Allgatherv(&J_raw[firstState],
                            lastState - firstState,
                            MPI_FLOAT,
                            J_raw,
                            nStatesPerProcess.data(),
                            displs.data(),
                            MPI_FLOAT,
                            MPI_COMM_WORLD
             );

             // Check for convergence by seaching the biggest change in J from all processors
             MPI_Allreduce(&error, &error, 1, MPI_FLOAT, MPI_MAX, MPI_COMM_WORLD);


 };