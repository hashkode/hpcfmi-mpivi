#define EIGEN_USE_BLAS
#define EIGEN_USE_LAPACKE

#include <chrono>
#include <iostream>
#include <mpi.h>
#include <string>

#include "npy.hpp"

#include "MpiViSchema01.h"
#include "MpiViSchema02.h"
#include "MpiViSchema03.h"
#include "MpiViUtility.h"
#include "verbose.h"

int main(int argc, char *argv[]) {
    // initialisation
    MpiViUtility::LogParameters logParameters{
            .startDatetime = MpiViUtility::datetime(),
            .tStart = std::chrono::system_clock::now(),
    };
    MpiViUtility::MpiParameters mpiParameters{};
    MpiViUtility::ViParameters viParameters{};

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiParameters.worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiParameters.worldRank);

    MpiViUtility::loadConfiguration(viParameters, mpiParameters, logParameters, &argc, argv);

    MpiViUtility::sync_Parameters(viParameters, mpiParameters);

    // mpi vi execution
    if (mpiParameters.nameSchema == "MpiViSchema01") {
        MpiViSchema01 schema;
        schema.ValueIteration(viParameters, mpiParameters, logParameters);
    } else if (mpiParameters.nameSchema == "MpiViSchema03") {
        MpiViSchema03 schema;
        schema.ValueIteration(viParameters, mpiParameters, logParameters);
    } else {
        throw std::invalid_argument("The specified MPI schema does not exist.");
    }

    MpiViUtility::saveResults(viParameters, mpiParameters, logParameters);

    MPI_Finalize();

    return 0;
}
