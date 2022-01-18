#define EIGEN_USE_BLAS
#define EIGEN_USE_LAPACKE

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <mpi.h>
#include <string>

#include "Eigen/Sparse"
#include "npy.hpp"

#include "MpiViSchema01.h"
#include "MpiViUtility.h"
#include "verbose.h"

//TODO: move to MpiViUtility class

// Loading npy files into std::vector<T>
template<typename T>
void loadNpy(std::vector<T> &data, const std::string &path, const std::string &filename) {

    std::vector<unsigned long> shape;
    bool fortranOrder;

    std::string ss = path + filename;

#ifdef VERBOSE_DEBUG
    // std::cout << ss << std::endl;
#endif

    npy::LoadArrayFromNumpy(ss, shape, fortranOrder, data);
}

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
            throw std::invalid_argument("Invalid number of arguments.");
        }
    }

    std::string configurationFile = argv[1];
    if (!std::filesystem::exists(configurationFile)) { throw std::invalid_argument("The specified configuration file does not exist."); }

    // get username for individual path
    const char *user = std::getenv("USER");
    std::string username(user);

    std::string basePath, dataSubPath;
    std::vector<int> indices, indptr;
    std::vector<float> data, jStar;

    MpiViUtility::loadConfiguration(configurationFile, viParameters, mpiParameters, logParameters, basePath, dataSubPath);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiParameters.worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiParameters.worldRank);

    MpiViUtility::loadParameters(viParameters, basePath + username + dataSubPath, "params.txt");

    loadNpy<float>(data, basePath + username + dataSubPath, "P_data.npy");
    loadNpy<int>(indices, basePath + username + dataSubPath, "P_indices.npy");
    loadNpy<int>(indptr, basePath + username + dataSubPath, "P_indptr.npy");
    loadNpy<float>(jStar, basePath + username + dataSubPath, "J_star_alpha_0_99.npy");

    std::vector<float> j;
    j.reserve(viParameters.NS);
    std::vector<int> pi;
    pi.reserve(viParameters.NS);

    // mpi vi execution
    auto tStartVi = std::chrono::system_clock::now();;
    if (mpiParameters.nameSchema == "MpiViSchema01") {
        MpiViSchema01 schema;
        schema.ValueIteration(j, data.data(), indices.data(), indptr.data(), pi, viParameters, mpiParameters, logParameters);
    }else{
        throw std::invalid_argument("The specified MPI schema does not exist.");
    }
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
