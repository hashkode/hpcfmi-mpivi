#include "MpiViSchema03.h"

#include <chrono>
#include <mpi.h>

#include "MpiViUtility.h"
#include "ValueIteration.h"
#include "verbose.h"

MpiViSchema03::MpiViSchema03() { MpiViSchema03::name = std::string(__func__); }

void MpiViSchema03::ValueIteration(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters) {
    //init
    const char *user = std::getenv("USER");
    mpiParameters.username = user;

    std::vector<int> indices, indptr;
    std::vector<float> data;

    MpiViUtility::loadNpy(data, std::string(mpiParameters.basePath + mpiParameters.username + mpiParameters.dataSubPath), std::string("P_data.npy"));
    MpiViUtility::loadNpy(indices, mpiParameters.basePath + mpiParameters.username + mpiParameters.dataSubPath, "P_indices.npy");
    MpiViUtility::loadNpy(indptr, mpiParameters.basePath + mpiParameters.username + mpiParameters.dataSubPath, "P_indptr.npy");

    std::vector<float> j;
    j.reserve(viParameters.NS);
    std::vector<int> pi;
    pi.reserve(viParameters.NS);

    auto valueIteration = Backend::ValueIteration();

    auto tStartVi = std::chrono::system_clock::now();

    std::vector<int> nStatesPerProcess, stateOffset;
    for (int iProcessor = 0; iProcessor < mpiParameters.worldSize; ++iProcessor) {
        if (iProcessor + 1 < mpiParameters.worldSize) nStatesPerProcess.push_back((viParameters.NS / mpiParameters.worldSize));
        else {
            nStatesPerProcess.push_back((viParameters.NS / mpiParameters.worldSize) + viParameters.NS % mpiParameters.worldSize);
        }

        if (iProcessor == 0) stateOffset.push_back(0);
        else {
            stateOffset.push_back(stateOffset[iProcessor - 1] + nStatesPerProcess[iProcessor - 1]);
        }
    }

    // Split up states for each rank
    viParameters.firstState = stateOffset[mpiParameters.worldRank];
    viParameters.lastState = stateOffset[mpiParameters.worldRank] + nStatesPerProcess[mpiParameters.worldRank] - 1;

#ifdef VERBOSE_DEBUG
    std::cout << "nStatesPerProcess:" << std::endl;
    for (int iStatesPerProcess: nStatesPerProcess) { std::cout << iStatesPerProcess << ", "; }
    std::cout << std::endl;

    std::cout << "stateOffset:" << std::endl;
    for (int iStateOffset: stateOffset) { std::cout << iStateOffset << ", "; }
    std::cout << std::endl;

    std::cout << "firstState: " << viParameters.firstState << ", lastState: " << viParameters.lastState << std::endl;
    std::cout << "world_size: " << mpiParameters.worldSize << ", world_rank: " << mpiParameters.worldRank << std::endl;
#endif

    float epsGlobal = 0;
    unsigned int iStep = 0;
    unsigned int conditionCount = 0;

    while (conditionCount < mpiParameters.conditionThreshold && iStep < mpiParameters.maxIterations) {
        iStep++;

        float epsStep = valueIteration.valueIteration(j.data(), data.data(), indices.data(), indptr.data(), pi.data(), viParameters);

        if (epsStep > epsGlobal) { epsGlobal = epsStep; };

        if (iStep % mpiParameters.comInterval == 0) {
            if (mpiParameters.worldRank != 0) {
                MPI_Ssend(j.data() + viParameters.firstState, nStatesPerProcess[mpiParameters.worldRank], MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
            } else {
                for (int iSource = 1; iSource < mpiParameters.worldSize; ++iSource) { MPI_Recv(j.data() + stateOffset[iSource], nStatesPerProcess[iSource], MPI_FLOAT, iSource, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); }
            }

            MPI_Bcast(j.data(), viParameters.NS, MPI_FLOAT, 0, MPI_COMM_WORLD);

            if (mpiParameters.worldRank != 0) {
                MPI_Ssend(&epsGlobal, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
            } else {
                std::vector<float> eps = {epsGlobal};
                eps.resize(mpiParameters.worldSize);
                for (int iSource = 1; iSource < mpiParameters.worldSize; ++iSource) { MPI_Recv(eps.data() + iSource, 1, MPI_FLOAT, iSource, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); }
                epsGlobal = *std::max_element(eps.begin(), eps.end());
            }

            MPI_Bcast(&epsGlobal, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

            if (epsGlobal < viParameters.eps) {
                conditionCount++;
            } else {
                conditionCount = 0;
            }
            epsGlobal = 0;
        }
    }

    MPI_Gatherv(pi.data() + viParameters.firstState, nStatesPerProcess[mpiParameters.worldRank], MPI_INT, pi.data(), nStatesPerProcess.data(), stateOffset.data(), MPI_INT, 0, MPI_COMM_WORLD);

    if (mpiParameters.worldRank == 0) {
        auto tEndVi = std::chrono::system_clock::now();

        logParameters.runtimeVi = (unsigned int) std::chrono::duration_cast<std::chrono::milliseconds>(tEndVi - tStartVi).count();
        logParameters.epsGlobal = epsGlobal;
        logParameters.steps = iStep;

        calculateMetrics(j, viParameters, mpiParameters, logParameters);
    }
}
std::string MpiViSchema03::GetName() { return this->name; }
