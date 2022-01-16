// A simple program to debug the backend code

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <mpi.h>
#include <string>

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
    MpiViUtility::LogParameters logParameters;
    logParameters.startDatetime = MpiViUtility::datetime();
    logParameters.filePath = "../results/";// TODO: parse from configuration file

    MpiViUtility::MpiParameters mpiParameters;
    mpiParameters.mpiViConfiguration = "local";// TODO: parse from configuration file

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiParameters.worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiParameters.worldRank);

    // get username for individual path
    const char *user = std::getenv("USER");
    std::string username(user);
    // construct path to tmp directory of current user
    std::string path = "/var/tmp/" + username;// TODO: add to configuration file
    // Add subpath to used data-set
    path += "/data/data_small/";// TODO: add to configuration file

#ifdef VERBOSE_DEBUG
    std::cout << path << std::endl;
#endif

    MpiViUtility::ViParameters viParameters;
    MpiViUtility::loadParameters(viParameters, path, "params.txt");// TODO: add to configuration file

#ifdef VERBOSE_INFO
    if (worldRank == 0) {
        std::cout << "Confusion " << viParameters.confusion_distance << std::endl;
        std::cout << "Fuel " << viParameters.fuel_capacity << std::endl;
        std::cout << "MaxU " << viParameters.max_controls << std::endl;
        std::cout << "N stars " << viParameters.number_stars << std::endl;
        std::cout << "NS " << viParameters.NS << std::endl;
        std::cout << "P Cols" << viParameters.cols << std::endl;
        std::cout << "P Rows" << viParameters.rows << std::endl;
        std::cout << "P data Size " << viParameters.data << std::endl;
        std::cout << "P indices Size " << viParameters.indices << std::endl;
        std::cout << "P indptr Size " << viParameters.indptr << std::endl;
    }
#endif
    MpiViSchema01 schema;// TODO: add to configuration file

    mpiParameters.comInterval = 1;       // TODO: add to configuration file
    mpiParameters.maxIterations = 150;   // TODO: add to configuration file
    mpiParameters.conditionThreshold = 5;// TODO: add to configuration file

    viParameters.doAsync = false;   // TODO: add to configuration file
    viParameters.alpha = .99;       // TODO: add to configuration file
    viParameters.eps = 1e-6;        // TODO: add to configuration file
    viParameters.maxIterations = 10;// TODO: add to configuration file

    std::vector<int> indices, indptr;
    std::vector<float> data, jStar;

    loadNpy<float>(data, path, "P_data.npy");
    loadNpy<int>(indices, path, "P_indices.npy");
    loadNpy<int>(indptr, path, "P_indptr.npy");
    loadNpy<float>(jStar, path, "J_star_alpha_0_99.npy");

    std::vector<float> j;
    j.reserve(viParameters.NS);
    std::vector<int> pi;
    pi.reserve(viParameters.NS);

    // mpi vi execution
    auto tStartVi = std::chrono::system_clock::now();
    schema.ValueIteration(j, data.data(), indices.data(), indptr.data(), pi, viParameters, mpiParameters, logParameters);
    auto tEndVi = std::chrono::system_clock::now();

    // measurement and logging
    if (mpiParameters.worldRank == 0) {
        Eigen::Map<Eigen::VectorXf> _j0(j.data(), viParameters.NS);
        Eigen::Map<Eigen::VectorXf> _jStar(jStar.data(), viParameters.NS);

        logParameters.jDiffMaxNorm = (_j0 - _jStar).template lpNorm<Eigen::Infinity>();
        logParameters.jDiffL2Norm = (_j0 - _jStar).template lpNorm<2>();
        logParameters.jDiffMSE = (_j0 - _jStar).array().square().sum() / _j0.size();

#ifdef VERBOSE_DEBUG
        for (int iState = 0; iState < 10; ++iState) { std::cout << iState << "\t\t>> j0: " << _j0[iState] << ";  \tjStar: " << _jStar[iState] << ";  \tpi: " << pi[iState] << std::endl; }

        for (int iState = viParameters.NS - 10; iState < (int) viParameters.NS; ++iState) { std::cout << iState << "\t\t>> j0: " << _j0[iState] << ";  \tjStar: " << _jStar[iState] << ";  \tpi: " << pi[iState] << std::endl; }
#endif
        logParameters.runtimeVi = (unsigned int) std::chrono::duration_cast<std::chrono::milliseconds>(tEndVi - tStartVi).count();
        logParameters.maxRSS = MpiViUtility::getMaxRSSUsage();

        logParameters.nameSchemaClass = schema.GetName();
        auto tEnd = std::chrono::system_clock::now();
        logParameters.runtime = (unsigned int) std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tStart).count();

        MpiViUtility::saveResults(mpiParameters, logParameters);

        std::cout << "epsGlobal: " << logParameters.epsGlobal << ", max norm (J - J*): " << logParameters.jDiffMaxNorm << ", l2 norm (J - J*): " << logParameters.jDiffL2Norm << ", MSE (J - J*): " << logParameters.jDiffMSE << std::endl;
        std::cout << "converged at: " << logParameters.steps << ", runtime: " << logParameters.runtime << "ms, MpiVi took: " << logParameters.runtimeVi << "ms, with maxRSSU: " << logParameters.maxRSS << "kb" << std::endl;
    }

    MPI_Finalize();

    return 0;
}
