#include "util.h"
#include "FirstSchema.h"
#include <mpi.h>

float FirstSchema::ValueIteration(std::vector<float> &j, float *pData, int *pIndices, int *pIndptr, unsigned int pNnz,
                                  std::vector<int> &pi,
                                  const float alpha, const int maxF, const int nStars, const int maxU,
                                  const float epsThreshold, const bool doAsync, const int maxIteration,
                                  const int comInterval) {
    int worldSize, worldRank;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

    int nIteration = 10;
    auto valueIteration = Backend::ValueIteration();

    // Split up states for each rank
    const int firstState = (pNnz / worldSize) * worldRank;
    const int lastState = (worldSize - 1 == worldRank) ? pNnz - 1 : (pNnz / worldSize) * (worldRank + 1) - 1;

#ifdef VERBOSE_DEBUG
    std::cout << "pNnz: " << pNnz << ", world_size: " << worldSize << ", world_rank: " << worldRank << std::endl;
    std::cout << "firstState: " << firstState << ", lastState: " << lastState << std::endl;
#endif

    std::vector<int> nStatesPerProcess, stateOffset;
    for (int iProcessor = 0; iProcessor < worldSize; ++iProcessor) {
        if (iProcessor + 1 < worldSize) nStatesPerProcess.push_back((pNnz / worldSize));
        else nStatesPerProcess.push_back((pNnz / worldSize) + pNnz % worldSize);

        if (iProcessor == 0) stateOffset.push_back(0);
        else stateOffset.push_back(stateOffset[iProcessor - 1] + nStatesPerProcess[iProcessor - 1]);
    }

    float epsGlobal = 0;
    int iStep = 0;
    int conditionCount = 0;
    int conditionThreshold = 5;
    
    while (conditionCount < conditionThreshold && iStep < maxIteration) {
        iStep++;

        float epsStep = valueIteration.valueIteration(j.data(), pData, pIndices, pIndptr, pNnz, pi.data(), alpha, maxF,
                                                      nStars, maxU,
                                                      epsThreshold, doAsync, nIteration, firstState, lastState);

        if (epsStep > epsGlobal) {
            epsGlobal = epsStep;
        };

        if (iStep % comInterval == 0) {
            MPI_Allgatherv(&j[firstState], lastState - firstState, MPI_FLOAT, j.data(), nStatesPerProcess.data(),
                           stateOffset.data(), MPI_FLOAT, MPI_COMM_WORLD);

            MPI_Allreduce(&epsGlobal, &epsGlobal, 1, MPI_FLOAT, MPI_MAX, MPI_COMM_WORLD);

            if (epsGlobal < epsThreshold) { conditionCount++; }
            else { conditionCount = 0; }

            epsGlobal = 0;
        }
    }

    if (worldRank == 0) {
        std::cout << "converged at: " << iStep << std::endl;
    }

    MPI_Gatherv(&pi[firstState], lastState - firstState, MPI_INT, pi.data(),
                nStatesPerProcess.data(), stateOffset.data(), MPI_INT, 0, MPI_COMM_WORLD);

    return epsGlobal,iStep;
}
std::string FirstSchema::GetName() {
    return typeid(*this).name();
}
