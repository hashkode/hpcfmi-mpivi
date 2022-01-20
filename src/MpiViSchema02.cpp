#include "MpiViSchema02.h"

#include <chrono>
#include <mpi.h>

#include "MpiViUtility.h"
#include "ValueIteration.h"
#include "verbose.h"

MpiViSchema02::MpiViSchema02() { MpiViSchema02::name = std::string(__func__); }

void MpiViSchema02::ValueIteration(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters) {
    //init
    const char *user = std::getenv("USER");
    mpiParameters.username = user;

    std::vector<int> indices, indptr;
    std::vector<float> data, jStar;

    std::vector<int> indptrOffsets, indptrBlockSizes;
    std::vector<int> dataOffsets, dataBlockSizes;
    indptrBlockSizes.resize(mpiParameters.worldSize);
    indptrOffsets.resize(mpiParameters.worldSize + 1);
    dataBlockSizes.resize(mpiParameters.worldSize);
    dataOffsets.resize(mpiParameters.worldSize);

    if (mpiParameters.worldRank == 0) {
        MpiViUtility::loadNpy(data, std::string(mpiParameters.basePath + mpiParameters.username + mpiParameters.dataSubPath), std::string("P_data.npy"));
        MpiViUtility::loadNpy(indices, mpiParameters.basePath + mpiParameters.username + mpiParameters.dataSubPath, "P_indices.npy");
        MpiViUtility::loadNpy(indptr, mpiParameters.basePath + mpiParameters.username + mpiParameters.dataSubPath, "P_indptr.npy");
        MpiViUtility::loadNpy(jStar, mpiParameters.basePath + mpiParameters.username + mpiParameters.dataSubPath, "J_star_alpha_0_99.npy");

        // split P
        // split indptr into #worldsize pieces -> indptrOffsets
        int indptrblockSize = (indptr.size() - 1) / mpiParameters.worldSize;
        indptrblockSize = indptrblockSize - indptrblockSize % viParameters.max_controls;
        std::fill(indptrBlockSizes.begin(), indptrBlockSizes.end(), indptrblockSize);

        if (mpiParameters.worldSize * indptrblockSize != (indptr.size() - 1)) { indptrBlockSizes[mpiParameters.worldSize - 1] = indptr.size() - ((mpiParameters.worldSize - 1) * indptrblockSize); }
        indptrOffsets[0] = 0;
        for (int i = 1; i < mpiParameters.worldSize; i++) { indptrOffsets[i] = indptrOffsets[i - 1] + indptrBlockSizes[i - 1]; }
        indptrOffsets[mpiParameters.worldSize] = indptr.size() - 1;
        // build dataOffsets according to indptr & indptrOffsets
        dataOffsets[0] = 0;
        dataBlockSizes[0] = indptr[indptrOffsets[1]];
        for (int i = 1; i < mpiParameters.worldSize; i++) {
            dataBlockSizes[i] = (indptr[indptrOffsets[i + 1]] - indptr[indptrOffsets[i]]);
            dataOffsets[i] = dataOffsets[i - 1] + dataBlockSizes[i - 1];
        }
    }

    // Broadcast Blocksizes
    if (mpiParameters.worldRank != 0) {
        indptrBlockSizes.resize(mpiParameters.worldSize);
        dataBlockSizes.resize(mpiParameters.worldSize);
    }

    MPI_Bcast(&indptrBlockSizes[0], mpiParameters.worldSize, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&dataBlockSizes[0], mpiParameters.worldSize, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&indptrOffsets[0], mpiParameters.worldSize + 1, MPI_INT, 0, MPI_COMM_WORLD);

    std::vector<int> localIndices, localIndptr;
    std::vector<float> localData;
    localIndptr.resize(indptrBlockSizes[mpiParameters.worldRank]);
    localData.resize(dataBlockSizes[mpiParameters.worldRank]);
    localIndices.resize(dataBlockSizes[mpiParameters.worldRank]);

    // Scatterv data-vectors with according Offset
    if (mpiParameters.worldRank == 0) {

        MPI_Scatterv(indptr.data(), indptrBlockSizes.data(), indptrOffsets.data(), MPI_INT, localIndptr.data(), indptrBlockSizes[mpiParameters.worldRank], MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatterv(data.data(), dataBlockSizes.data(), dataOffsets.data(), MPI_FLOAT, localData.data(), dataBlockSizes[mpiParameters.worldRank], MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Scatterv(indices.data(), dataBlockSizes.data(), dataOffsets.data(), MPI_INT, localIndices.data(), dataBlockSizes[mpiParameters.worldRank], MPI_INT, 0, MPI_COMM_WORLD);

        int length = 0;
        for (int i = 1; i <= mpiParameters.worldSize; i++) { length += indptrBlockSizes[i]; }
        std::vector<int> appendVector;
        appendVector.resize(length);
        std::fill(appendVector.begin(), appendVector.end(), localData.size());
        localIndptr.insert(localIndptr.end(), appendVector.begin(), appendVector.end());
    } else {
        MPI_Scatterv(NULL, NULL, NULL, MPI_INT, localIndptr.data(), indptrBlockSizes[mpiParameters.worldRank], MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatterv(NULL, NULL, NULL, MPI_FLOAT, localData.data(), dataBlockSizes[mpiParameters.worldRank], MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Scatterv(NULL, NULL, NULL, MPI_INT, localIndices.data(), dataBlockSizes[mpiParameters.worldRank], MPI_INT, 0, MPI_COMM_WORLD);

        if (mpiParameters.worldRank == (mpiParameters.worldSize - 1)) {
            int length = 0;
            for (int i = 0; i < mpiParameters.worldRank; i++) { length += indptrBlockSizes[i]; }
            std::vector<int> appendVector;
            appendVector.resize(length);
            std::fill(appendVector.begin(), appendVector.end(), 0);
            Eigen::Map<Eigen::VectorXi> _localIndptr(localIndptr.data(), localIndptr.size());
            _localIndptr = _localIndptr.array() - localIndptr[0];
            localIndptr.insert(localIndptr.begin(), appendVector.begin(), appendVector.end());
        } else {
            int length = 0;
            for (int i = 0; i < mpiParameters.worldRank; i++) { length += indptrBlockSizes[i]; }
            std::vector<int> appendVector;
            appendVector.resize(length);
            std::fill(appendVector.begin(), appendVector.end(), 0);
            Eigen::Map<Eigen::VectorXi> _localIndptr(localIndptr.data(), localIndptr.size());
            _localIndptr = _localIndptr.array() - localIndptr[0];
            localIndptr.insert(localIndptr.begin(), appendVector.begin(), appendVector.end());

            length = 0;
            for (int i = mpiParameters.worldRank + 1; i <= mpiParameters.worldSize; i++) { length += indptrBlockSizes[i]; }
            appendVector.clear();
            appendVector.resize(length);
            std::fill(appendVector.begin(), appendVector.end(), localData.size());
            localIndptr.insert(localIndptr.end(), appendVector.begin(), appendVector.end());
        }
    }

    std::vector<float> j;
    j.reserve(viParameters.NS);
    std::vector<int> pi;
    pi.reserve(viParameters.NS);

    auto valueIteration = Backend::ValueIteration();
    auto tStartVi = std::chrono::system_clock::now();

#ifdef VERBOSE_DEBUG
    std::cout << "world_size: " << mpiParameters.worldSize << ", world_rank: " << mpiParameters.worldRank << std::endl;
    std::cout << "firstState: " << viParameters.firstState << ", lastState: " << viParameters.lastState << std::endl;
#endif

    std::vector<int> nStatesPerProcess, stateOffset;
    nStatesPerProcess.resize(mpiParameters.worldSize);
    stateOffset.resize(mpiParameters.worldSize);
    for (int iProcessor = 0; iProcessor < mpiParameters.worldSize; ++iProcessor) {
        int states = 0;
        if (iProcessor == mpiParameters.worldSize - 1) {
            states = (indptrOffsets[iProcessor + 1] - indptrOffsets[iProcessor]) / viParameters.max_controls;
            nStatesPerProcess[iProcessor] = states;
        } else {
            states = (indptrOffsets[iProcessor + 1] - indptrOffsets[iProcessor]) / viParameters.max_controls;
            nStatesPerProcess[iProcessor] = states;
        }

        if (iProcessor == 0) stateOffset[0] = 0;
        else
            stateOffset[iProcessor] = (stateOffset[iProcessor - 1] + nStatesPerProcess[iProcessor - 1] - 1);
    }

    viParameters.firstState = stateOffset[mpiParameters.worldRank];
    viParameters.lastState = (mpiParameters.worldSize - 1 == mpiParameters.worldRank) ? viParameters.NS - 1 : stateOffset[mpiParameters.worldRank + 1];

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

        float epsStep = valueIteration.valueIteration(j.data(), localData.data(), localIndices.data(), localIndptr.data(), pi.data(), viParameters);

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

    logParameters.runtimeVi = (unsigned int) std::chrono::duration_cast<std::chrono::milliseconds>(tEndVi - tStartVi).count();
    logParameters.epsGlobal = epsGlobal;
    logParameters.steps = iStep;

    calculateMetrics(j, jStar, viParameters, mpiParameters, logParameters);//*/
}

std::string MpiViSchema02::GetName() { return this->name; }
