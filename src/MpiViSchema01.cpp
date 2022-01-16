#include "MpiViSchema01.h"

#include <mpi.h>

#include "MpiViUtility.h"
#include "verbose.h"

MpiViSchema01::MpiViSchema01() { name = std::string(__func__); }

void MpiViSchema01::ValueIteration(std::vector<float> &j, float *pData, int *pIndices, int *pIndptr, std::vector<int> &pi, MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters) {
    auto valueIteration = Backend::ValueIteration();

    // Split up states for each rank
    viParameters.firstState = (viParameters.NS / mpiParameters.worldSize) * mpiParameters.worldRank;
    viParameters.lastState = (mpiParameters.worldSize - 1 == mpiParameters.worldRank) ? viParameters.NS - 1 : (viParameters.NS / mpiParameters.worldSize) * (mpiParameters.worldRank + 1) - 1;

#ifdef VERBOSE_DEBUG
    std::cout << "pNnz: " << pNnz << ", world_size: " << mpiParameters.worldSize << ", world_rank: " << mpiParameters.worldRank << std::endl;
    std::cout << "firstState: " << firstState << ", lastState: " << lastState << std::endl;
#endif

    std::vector<int> nStatesPerProcess, stateOffset;
    for (int iProcessor = 0; iProcessor < mpiParameters.worldSize; ++iProcessor) {
        if (iProcessor + 1 < mpiParameters.worldSize) nStatesPerProcess.push_back((viParameters.NS / mpiParameters.worldSize));
        else
            nStatesPerProcess.push_back((viParameters.NS / mpiParameters.worldSize) + viParameters.NS % mpiParameters.worldSize);

        if (iProcessor == 0) stateOffset.push_back(0);
        else
            stateOffset.push_back(stateOffset[iProcessor - 1] + nStatesPerProcess[iProcessor - 1]);
    }

#ifdef VERBOSE_DEBUG
    std::cout << "nStatesPerProcess:" << std::endl;
    for (int iStatesPerProcess: nStatesPerProcess) { std::cout << iStatesPerProcess << ", "; }
    std::cout << std::endl;

    std::cout << "stateOffset:" << std::endl;
    for (int iStateOffset: stateOffset) { std::cout << iStateOffset << ", "; }
    std::cout << std::endl;
#endif

    float epsGlobal = 0;
    int iStep = 0;
    int conditionCount = 0;

    while (conditionCount < mpiParameters.conditionThreshold && iStep < mpiParameters.maxIterations) {
        iStep++;

        float epsStep = valueIteration.valueIteration(j.data(), pData, pIndices, pIndptr, pi.data(), viParameters);

        if (epsStep > epsGlobal) { epsGlobal = epsStep; };

        if (iStep % mpiParameters.comInterval == 0) {
            MPI_Allgatherv(&j[viParameters.firstState], nStatesPerProcess[mpiParameters.worldRank], MPI_FLOAT, j.data(), nStatesPerProcess.data(), stateOffset.data(), MPI_FLOAT, MPI_COMM_WORLD);

            MPI_Allreduce(&epsGlobal, &epsGlobal, 1, MPI_FLOAT, MPI_MAX, MPI_COMM_WORLD);

            if (epsGlobal < viParameters.eps) {
                conditionCount++;
            } else {
                conditionCount = 0;
            }

            epsGlobal = 0;
        }
    }

    MPI_Gatherv(&pi[viParameters.firstState], nStatesPerProcess[mpiParameters.worldRank], MPI_INT, pi.data(), nStatesPerProcess.data(), stateOffset.data(), MPI_INT, 0, MPI_COMM_WORLD);
    logParameters.epsGlobal = epsGlobal;
    logParameters.steps = iStep;
}

std::string MpiViSchema01::GetName() { return this->name; }