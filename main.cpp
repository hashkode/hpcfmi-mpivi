// A simple program to debug the backend code

#include <string>
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <mpi.h>

#include "npy.hpp"

#include "verbose.h"
#include "MpiViUtility.h"
#include "MpiViSchema01.h"

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
    MPI_Init(&argc, &argv);
    int worldSize, worldRank;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

    // get username for individual path
    const char *user = std::getenv("USER");
    std::string username(user);
    // construct path to tmp directory of current user
    std::string path = "/var/tmp/" + username; // TODO: add to configuration file
    // Add subpath to used data-set
    path += "/data/data_small/"; // TODO: add to configuration file

#ifdef VERBOSE_DEBUG
    std::cout << path << std::endl;
#endif

    MpiViUtility::Parameters p = MpiViUtility::loadParameters(path, "params.txt"); // TODO: add to configuration file

#ifdef VERBOSE_INFO
    if (worldRank == 0) {
        std::cout << "Confusion " << p.confusion_distance << std::endl;
        std::cout << "Fuel " << p.fuel_capacity << std::endl;
        std::cout << "MaxU " << p.max_controls << std::endl;
        std::cout << "N stars " << p.number_stars << std::endl;
        std::cout << "NS " << p.NS << std::endl;
        std::cout << "P Cols" << p.cols << std::endl;
        std::cout << "P Cols" << p.rows << std::endl;
        std::cout << "P data Size " << p.data << std::endl;
        std::cout << "P indices Size " << p.indices << std::endl;
        std::cout << "P indptr Size " << p.indptr << std::endl;
    }
#endif
    std::vector<int> iStepVector;
    std::vector<float> durationVector;
    std::vector<float> jDiffsMaxNorm;
    std::vector<float> jDiffsL2Norm;
    std::vector<float> jDiffsMSE;
    std::vector<long> maxRSSs;
    MpiViSchema01 schema; // TODO: add to configuration file

    int nCycle = 3; // TODO: add to configuration file
    int comInterval = 1; // TODO: add to configuration file
    int maxIterations = 150; // TODO: add to configuration file
    bool doAsync = false; // TODO: add to configuration file

    for (int iCycle = 0; iCycle < nCycle; ++iCycle) {
        std::vector<int> indices, indptr;
        std::vector<float> data, jStar;

        loadNpy<float>(data, path, "P_data.npy");
        loadNpy<int>(indices, path, "P_indices.npy");
        loadNpy<int>(indptr, path, "P_indptr.npy");
        loadNpy<float>(jStar, path, "J_star_alpha_0_99.npy");

        std::vector<float> j;
        j.reserve(p.NS);
        std::vector<int> pi;
        pi.reserve(p.NS);
        float alpha = .99; // TODO: add to configuration file
        float eps = 1e-6; // TODO: add to configuration file

        auto tStart = std::chrono::system_clock::now();


        auto[epsGlobal, iStep] = schema.ValueIteration(j, data.data(), indices.data(), indptr.data(), p.NS, pi, alpha,
                                                       p.fuel_capacity, p.number_stars, p.max_controls, eps, doAsync,
                                                       maxIterations, comInterval);

        auto tEnd = std::chrono::system_clock::now();


        if (worldRank == 0) {
            Eigen::Map<Eigen::VectorXf> _j0(j.data(), p.NS);
            Eigen::Map<Eigen::VectorXf> _jStar(jStar.data(), p.NS);

            float jDiffMaxNorm = (_j0 - _jStar).template lpNorm<Eigen::Infinity>();
            float jDiffL2Norm = (_j0 - _jStar).template lpNorm<2>();
            float jDiffMSE = (_j0 - _jStar).array().square().sum() / _j0.size();

#ifdef VERBOSE_DEBUG
            for (int iCycle = 0; iCycle < 10; ++iCycle) {
                std::cout << iCycle << "\t\t>> j0: " << _j0[iCycle] << ";  \tjStar: " << _jStar[iCycle] << ";  \tpi: " << pi[iCycle] << std::endl;
            }

            for (int iCycle = p.NS - 10; iCycle < (int) p.NS; ++iCycle) {
                std::cout << iCycle << "\t\t>> j0: " << _j0[iCycle] << ";  \tjStar: " << _jStar[iCycle] << ";  \tpi: " << pi[iCycle] << std::endl;
            }
#endif
            float time = (float) std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tStart).count();
            iStepVector.push_back(iStep);
            durationVector.push_back(time);
            jDiffsMaxNorm.push_back(jDiffMaxNorm);
            jDiffsL2Norm.push_back(jDiffL2Norm);
            jDiffsMSE.push_back(jDiffMSE);
            maxRSSs.push_back(MpiViUtility::getMaxRSSUsage());

            std::cout << "epsGlobal: " << epsGlobal << ", max norm (J - J*): " << jDiffMaxNorm << ", l2 norm (J - J*): "
                      << jDiffL2Norm << ", MSE (J - J*): " << jDiffMSE << std::endl;
            std::cout << "This took " << std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tStart).count()
                      << "ms" << std::endl;
        }
    }

    if (worldRank == 0) {
        std::string nameClass = schema.GetName();
        MpiViUtility::saveResults(iStepVector, durationVector, jDiffsMaxNorm, jDiffsL2Norm, jDiffsMSE, maxRSSs,
                                  comInterval, nameClass);
    }

    MPI_Finalize();

    return 0;
}
