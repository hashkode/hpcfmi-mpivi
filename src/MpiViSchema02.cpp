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
    indptrOffsets.resize(mpiParameters.worldSize+1);
    dataBlockSizes.resize(mpiParameters.worldSize);
    dataOffsets.resize(mpiParameters.worldSize);

    if (mpiParameters.worldRank == 0) {
        MpiViUtility::loadNpy(data, std::string(mpiParameters.basePath + mpiParameters.username + mpiParameters.dataSubPath), std::string("P_data.npy"));
        MpiViUtility::loadNpy(indices, mpiParameters.basePath + mpiParameters.username + mpiParameters.dataSubPath, "P_indices.npy");
        MpiViUtility::loadNpy(indptr, mpiParameters.basePath + mpiParameters.username + mpiParameters.dataSubPath, "P_indptr.npy");
        MpiViUtility::loadNpy(jStar, mpiParameters.basePath + mpiParameters.username + mpiParameters.dataSubPath, "J_star_alpha_0_99.npy");

        std::cout << "size indices: " << indices.size() << std::endl;
        std::cout << "size indptr: " << indptr.size() << std::endl;
        std::cout << "size data: " << data.size() << std::endl;

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
        //    dataBlockSizes[mpiParameters.worldSize] = data.size()-indptr[indptrOffsets[mpiParameters.worldSize]];
        //    dataOffsets[mpiParameters.worldSize] = data.size();
    }
    if (mpiParameters.worldRank == 0) {
        for (int i = 0; i < mpiParameters.worldSize; i++) {
            std::cout << "indptr in block " << i << " : " << std::endl;
            for (int j = 0; j < indptrBlockSizes[i]; j++) { std::cout << indptr[j + indptrOffsets[i]] << ", "; }
            std::cout << std::endl;
        }
        for (int i = 0; i < mpiParameters.worldSize; i++) {
            std::cout << "indices in block " << i << " : " << std::endl;
            for (int j = 0; j < dataBlockSizes[i]; j++) { std::cout << indices[j + dataOffsets[i]] << ", "; }
            std::cout << std::endl;
        }
        for (int i = 0; i < mpiParameters.worldSize; i++) {
            std::cout << "data in block " << i << " : " << std::endl;
            for (int j = 0; j < dataBlockSizes[i]; j++) { std::cout << data[j + dataOffsets[i]] << ", "; }
            std::cout << std::endl;
        }
        for (int i = 0; i < mpiParameters.worldSize; i++) { std::cout << "indptrblocksizes in block " << i << " : " << indptrBlockSizes[i] << std::endl; }
        for (int i = 0; i <= mpiParameters.worldSize; i++) { std::cout << "indptroffsets in block " << i << " : " << indptrOffsets[i] << std::endl; }
        for (int i = 0; i < mpiParameters.worldSize; i++) { std::cout << "idatablocksizes in block " << i << " : " << dataBlockSizes[i] << std::endl; }
        for (int i = 0; i < mpiParameters.worldSize; i++) { std::cout << "dataoffsets in block " << i << " : " << dataOffsets[i] << std::endl; }
        int sum = 0;
        for (int i = 0; i < mpiParameters.worldSize; i++) { sum += indptrBlockSizes[i]; }
        std::cout << indptr.size() << " and sum " << sum << std::endl;
    }
    // Broadcast Blocksizes
    if (mpiParameters.worldRank != 0) {
        indptrBlockSizes.resize(mpiParameters.worldSize);
        dataBlockSizes.resize(mpiParameters.worldSize);
    }

    MPI_Bcast(&indptrBlockSizes[0], mpiParameters.worldSize, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&dataBlockSizes[0], mpiParameters.worldSize, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&indptrOffsets[0], mpiParameters.worldSize+1, MPI_INT, 0, MPI_COMM_WORLD);

    std::cout << "rank " << mpiParameters.worldRank << ", size indpt BS " << indptrBlockSizes.size() << ", val " << indptrBlockSizes[mpiParameters.worldRank] << std::endl;
    std::cout << "rank " << mpiParameters.worldRank << ", size data BS " << dataBlockSizes.size() << ", val " << dataBlockSizes[mpiParameters.worldRank] << std::endl;

    std::vector<int> localIndices, localIndptr;
    std::vector<float> localData;
    localIndptr.resize(indptrBlockSizes[mpiParameters.worldRank]);
    localData.resize(dataBlockSizes[mpiParameters.worldRank]);
    localIndices.resize(dataBlockSizes[mpiParameters.worldRank]);

    // Scatterv data-vectors with according Offset
    if (mpiParameters.worldRank==0) {

        MPI_Scatterv(indptr.data(), indptrBlockSizes.data(), indptrOffsets.data(), MPI_INT, localIndptr.data(), indptrBlockSizes[mpiParameters.worldRank], MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatterv(data.data(), dataBlockSizes.data(), dataOffsets.data(), MPI_FLOAT, localData.data(), dataBlockSizes[mpiParameters.worldRank], MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Scatterv(indices.data(), dataBlockSizes.data(), dataOffsets.data(), MPI_INT, localIndices.data(), dataBlockSizes[mpiParameters.worldRank], MPI_INT, 0, MPI_COMM_WORLD);

        int length = 0;
        for (int i = 1; i <= mpiParameters.worldSize; i++) { length += indptrBlockSizes[i]; }
        std::vector<int> append_vector;
        append_vector.resize(length);
        std::fill(append_vector.begin(), append_vector.end(), localData.size());
        localIndptr.insert(localIndptr.end(), append_vector.begin(), append_vector.end());
    } else {
        //localIndptr.resize(indptrBlockSizes[mpiParameters.worldRank]);
        //localData.resize(dataBlockSizes[mpiParameters.worldRank]);
        //localIndices.resize(dataBlockSizes[mpiParameters.worldRank]);
        MPI_Scatterv(NULL, NULL, NULL, MPI_INT, localIndptr.data(), indptrBlockSizes[mpiParameters.worldRank], MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatterv(NULL, NULL, NULL, MPI_FLOAT, localData.data(), dataBlockSizes[mpiParameters.worldRank], MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Scatterv(NULL, NULL, NULL, MPI_INT, localIndices.data(), dataBlockSizes[mpiParameters.worldRank], MPI_INT, 0, MPI_COMM_WORLD);
///*
        if (mpiParameters.worldRank == (mpiParameters.worldSize-1)) {
            int length = 0;
            for (int i = 0; i < mpiParameters.worldRank; i++) { length += indptrBlockSizes[i]; }
            std::vector<int> append_vector;
            append_vector.resize(length);
            std::fill(append_vector.begin(), append_vector.end(), 0);
            Eigen::Map<Eigen::VectorXi> _localIndptr(localIndptr.data(), localIndptr.size());
            _localIndptr = _localIndptr.array() - localIndptr[0];
            localIndptr.insert(localIndptr.begin(), append_vector.begin(), append_vector.end());
        } else {
            int length = 0;
            for (int i = 0; i < mpiParameters.worldRank; i++) { length += indptrBlockSizes[i]; }
            std::vector<int> append_vector;
            append_vector.resize(length);
            std::fill(append_vector.begin(), append_vector.end(), 0);
            Eigen::Map<Eigen::VectorXi> _localIndptr(localIndptr.data(), localIndptr.size());
            _localIndptr = _localIndptr.array() - localIndptr[0];
            localIndptr.insert(localIndptr.begin(), append_vector.begin(), append_vector.end());

            length = 0;
            for (int i = mpiParameters.worldRank + 1; i <= mpiParameters.worldSize; i++) { length += indptrBlockSizes[i]; }
            append_vector.clear();
            append_vector.resize(length);
            std::fill(append_vector.begin(), append_vector.end(), localData.size());
            localIndptr.insert(localIndptr.end(), append_vector.begin(), append_vector.end());
        }//*/
    }

/*
    if (mpiParameters.worldRank == 1) {
        std::cout << "first state in rank " << mpiParameters.worldRank << " : "<< viParameters.firstState <<  std::endl;
        std::cout << "last state in rank " << mpiParameters.worldRank << " : "<< viParameters.lastState <<  std::endl;
        std::cout << "size localindptr in rank " << mpiParameters.worldRank << " : "<< localIndptr.size() <<  std::endl;
        std::cout << "size localindices in rank " << mpiParameters.worldRank << " : "<< localIndices.size() << std::endl;
        std::cout << "size localData in rank " << mpiParameters.worldRank << " : "<< localData.size()  << std::endl;

        std::cout << "localindptr in rank " << mpiParameters.worldRank << " : " << std::endl;
        for (int j = 0; j < localIndptr.size(); j++) {
            std::cout << localIndptr[j] << ", ";
        }
        std::cout << std::endl;
        std::cout << "localindices in rank " << mpiParameters.worldRank << " : " << std::endl;
        for (int i = 0; i < localIndices.size(); i++) {
            std::cout << localIndices[i] << ", ";
        }
        std::cout << std::endl;
        std::cout << "localData in rank " << mpiParameters.worldRank << " : " << std::endl;
        for (int i = 0; i < localData.size(); i++) {
            std::cout << localData[i] << ", ";
        }
        std::cout << std::endl;
    } //*/

    std::cout << "were here " <<viParameters.NS<< std::endl;
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
    for (int iProcessor = 0; iProcessor < mpiParameters.worldSize; ++iProcessor) {
        int states = 0;
        if(iProcessor == mpiParameters.worldSize-1)
        {
            states = (indptrOffsets[mpiParameters.worldRank + 1] - indptrOffsets[mpiParameters.worldRank])/viParameters.max_controls;
            nStatesPerProcess.push_back(states);
        } else {
            states = (indptrOffsets[mpiParameters.worldRank + 1] - indptrOffsets[mpiParameters.worldRank])/viParameters.max_controls;
            nStatesPerProcess.push_back(states);
        }

        if (iProcessor == 0) stateOffset.push_back(0);
        else
            stateOffset.push_back(stateOffset[iProcessor - 1] + nStatesPerProcess[iProcessor - 1] - 1);
    }
    std::cout << "nStatesPerProcess " <<  nStatesPerProcess[mpiParameters.worldRank] << " and stateOffset " << stateOffset[mpiParameters.worldRank] << std::endl;
    viParameters.firstState = stateOffset[mpiParameters.worldRank];
    viParameters.lastState = (mpiParameters.worldSize - 1 == mpiParameters.worldRank) ? viParameters.NS - 1 : stateOffset[mpiParameters.worldRank + 1];

    std::cout << "rank " << mpiParameters.worldRank << " and firstState " << viParameters.firstState << " and lastState " << viParameters.lastState <<std::endl;

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
