#define EIGEN_USE_BLAS
#define EIGEN_USE_LAPACKE

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <mpi.h>
#include <string>

#include "npy.hpp"

#include "MpiViSchema01.h"
#include "MpiViUtility.h"
#include "verbose.h"

int main(int argc, char *argv[]) {
    // initialisation
    auto tStart = std::chrono::system_clock::now();
    MpiViUtility::LogParameters logParameters{.startDatetime = MpiViUtility::datetime()};
    MpiViUtility::MpiParameters mpiParameters{};
    MpiViUtility::ViParameters viParameters{};

    // commandline argument checks
    if (mpiParameters.worldRank == 0) {
        if (argc != 2) {
            std::cout << "You entered the wrong number of arguments: " << argc << " arguments instead of 2." << std::endl;
            std::cout << "Intended use is: mpirun <options> mpi-vi <configurationpath>" << std::endl;
            for (int i = 0; i < argc; ++i) { std::cout << argv[i] << "\n"; }
            //throw std::invalid_argument("Invalid number of arguments.");
        }
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiParameters.worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiParameters.worldRank);

    if (true) {// replace with <mpiParameters.worldRank == 0> if BCast for complete structs is implemented
        // get username for individual path
        const char *user = std::getenv("USER");
        mpiParameters.username = user;

        mpiParameters.configurationFile = argv[1];
        if (!std::filesystem::exists(mpiParameters.configurationFile)) { throw std::invalid_argument("The specified configuration file does not exist."); }
        MpiViUtility::loadConfiguration(viParameters, mpiParameters, logParameters);
        MpiViUtility::loadParameters(viParameters, mpiParameters.basePath + mpiParameters.username + mpiParameters.dataSubPath, "params.txt");
    }

    int schemaKey;

    if (mpiParameters.worldRank == 0) {
        if (mpiParameters.nameSchema == "MpiViSchema01") { schemaKey = 1; }
    }
    MPI_Bcast(&schemaKey, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // mpi vi execution
    if (schemaKey == 1) {
        MpiViSchema01 schema;
        schema.ValueIteration(viParameters, mpiParameters, logParameters);
    } else {
        throw std::invalid_argument("The specified MPI schema does not exist.");
    }

    // measurement and logging
    if (mpiParameters.worldRank == 0) {
        logParameters.maxRSS = MpiViUtility::getMaxRSSUsage();

        auto tEnd = std::chrono::system_clock::now();
        logParameters.runtime = (unsigned int) std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tStart).count();

        MpiViUtility::saveResults(mpiParameters, logParameters);

        std::cout << "epsGlobal: " << logParameters.epsGlobal << ", max norm (J - J*): " << logParameters.jDiffMaxNorm << ", l2 norm (J - J*): " << logParameters.jDiffL2Norm << ", MSE (J - J*): " << logParameters.jDiffMSE << std::endl;
        std::cout << "converged at: " << logParameters.steps << ", runtime: " << logParameters.runtime << "ms, MpiVi took: " << logParameters.runtimeVi << "ms, with maxRSSU: " << logParameters.maxRSS << "kb" << std::endl;
    }

    MPI_Finalize();

    return 0;
}
