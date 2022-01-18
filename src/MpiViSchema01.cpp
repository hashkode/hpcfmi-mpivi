#include "MpiViSchema01.h"

#include <chrono>
#include <mpi.h>

#include "MpiViSchema.h"
#include "MpiViSchema01.h"
#include "MpiViUtility.h"
#include "ValueIteration.h"
#include "verbose.h"

MpiViSchema01::MpiViSchema01() { MpiViSchema01::name = std::string(__func__); }
void MpiViSchema01::ValueIteration(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters) {
    auto tStartVi = std::chrono::system_clock::now();
    //init
    const char *user = std::getenv("USER");
    mpiParameters.username = user;

    std::vector<int> indices, indptr;
    std::vector<float> data, jStar;

    MpiViUtility::loadNpy(data, std::string(mpiParameters.basePath + mpiParameters.username + mpiParameters.dataSubPath), std::string("P_data.npy"));
    MpiViUtility::loadNpy(indices, mpiParameters.basePath + mpiParameters.username + mpiParameters.dataSubPath, "P_indices.npy");
    MpiViUtility::loadNpy(indptr, mpiParameters.basePath + mpiParameters.username + mpiParameters.dataSubPath, "P_indptr.npy");
    MpiViUtility::loadNpy(jStar, mpiParameters.basePath + mpiParameters.username + mpiParameters.dataSubPath, "J_star_alpha_0_99.npy");

    std::vector<float> j;
    j.reserve(viParameters.NS);
    std::vector<int> pi;
    pi.reserve(viParameters.NS);

    auto valueIteration = Backend::ValueIteration();

    // Split up states for each rank
    viParameters.firstState = (viParameters.NS / mpiParameters.worldSize) * mpiParameters.worldRank;
    viParameters.lastState = (mpiParameters.worldSize - 1 == mpiParameters.worldRank) ? viParameters.NS - 1 : (viParameters.NS / mpiParameters.worldSize) * (mpiParameters.worldRank + 1) - 1;

#ifdef VERBOSE_DEBUG
    std::cout << "world_size: " << mpiParameters.worldSize << ", world_rank: " << mpiParameters.worldRank << std::endl;
    std::cout << "firstState: " << viParameters.firstState << ", lastState: " << viParameters.lastState << std::endl;
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
    unsigned int iStep = 0;
    unsigned int conditionCount = 0;

    while (conditionCount < mpiParameters.conditionThreshold && iStep < mpiParameters.maxIterations) {
        iStep++;

        float epsStep = valueIteration.valueIteration(j.data(), data.data(), indices.data(), indptr.data(), pi.data(), viParameters);

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

    auto tEndVi = std::chrono::system_clock::now();

    // measurement and logging
    if (mpiParameters.worldRank == 0) {
        Eigen::Map<Eigen::VectorXf> _j0(j.data(), viParameters.NS);
        Eigen::Map<Eigen::VectorXf> _jStar(jStar.data(), viParameters.NS);
#ifdef VERBOSE_DEBUG
        for (int iState = 0; iState < 10; ++iState) { std::cout << iState << "\t\t>> j0: " << _j0[iState] << ";  \tjStar: " << _jStar[iState] << ";  \tpi: " << pi[iState] << std::endl; }

        for (unsigned int iState = viParameters.NS - 10; iState < (int) viParameters.NS; ++iState) { std::cout << iState << "\t\t>> j0: " << _j0[iState] << ";  \tjStar: " << _jStar[iState] << ";  \tpi: " << pi[iState] << std::endl; }
#endif

        logParameters.runtimeVi = (unsigned int) std::chrono::duration_cast<std::chrono::milliseconds>(tEndVi - tStartVi).count();

        logParameters.jDiffMaxNorm = (_j0 - _jStar).template lpNorm<Eigen::Infinity>();
        logParameters.jDiffL2Norm = (_j0 - _jStar).template lpNorm<2>();
        logParameters.jDiffMSE = (_j0 - _jStar).array().square().sum() / (float) _j0.size();
        logParameters.epsGlobal = epsGlobal;
        logParameters.steps = iStep;
    }
}
std::string MpiViSchema01::GetName() { return this->name; }