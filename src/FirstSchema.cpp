#include "FirstSchema.h"
#include <mpi.h>
#include "simulator.cpp"

void FirstSchema::ValueIteration(float *j, float *pData, int *pIndices, int *pIndptr, unsigned int pNnz, int *pi,
                                 const float alpha, const int maxF, const int nStars, const int maxU,
                                 const float epsThreshold, const bool doAsync, int maxIteration, int commPeriode) {
    // init open mpi world and rank
    int world_size, world_rank;
    std::vector<float> _j;
    _j.reserve(pNnz);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // Number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // Rank of this process

    int nIteration = 10;
    // Split up states for each rank
    const int firstState = (pNnz / world_size) * world_rank;
    int ls;
    if (world_size - 1 == world_rank){
        ls = pNnz - 1;
    }
    else{
        ls = (pNnz / world_size) * (world_rank + 1) - 1;
    }
    const int lastState = ls;
    std::cout << "pNnz: " << pNnz << ", world_size: " << world_size << ", world_rank: " << world_rank << std::endl;
    std::cout << "firstState: " << firstState << ", lastState: " << lastState << std::endl;


    // set number of states per process and displacements of subvectors
    std::vector<int> nStatesPerProcess, displs;    // Number of states and Displacement of sub vectors for each process
    for (int i = 0; i < world_size; ++i) {
        if (i + 1 < world_size) nStatesPerProcess.push_back((pNnz / world_size));
        else nStatesPerProcess.push_back((pNnz / world_size) + pNnz % world_size);

        if (i == 0) displs.push_back(0);
        else displs.push_back(displs[i - 1] + nStatesPerProcess[i - 1]);
    }

    // Keeps track of the change in J vector
    float error = 0;
    for (unsigned int t = 0; t < maxIteration; ++t) {
        // Compute one value iteration step for a range of states
        float epsGlobal = Backend::valueIteration(&_j[0], pData, pIndices, pIndptr, pNnz, pi, alpha, maxF, nStars, maxU, epsThreshold,
                                                  doAsync, nIteration, firstState, lastState);

        // Store value of biggest change that appeared while updating J
        if (epsGlobal > error) {
            error = epsGlobal;
        };

        // Exchange results every comm_period cycles
        if (t % commPeriode == 0) {
            // Merge local updates together
            float *J_raw = _j.data();
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

        }
    }

    j = &_j[0];
}
