#define EIGEN_USE_BLAS
#define EIGEN_USE_LAPACKE

#include "Eigen/Sparse"
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <mpi.h>
#include <string>

#include "npy.hpp"
#include "yaml-cpp/yaml.h"

#include "MpiViSchema.h"
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
    //logParameters.startDatetime = MpiViUtility::datetime();

    MpiViUtility::MpiParameters mpiParameters;


    // yaml test
    std::string configurationFile = argv[1];

    if (!std::filesystem::exists(configurationFile)) { throw std::invalid_argument("The specified configuration file does not exist."); }

    MpiViUtility::ViParameters viParameters{};
    std::vector<int> indices, indptr;
    std::vector<float> data, jStar;

    YAML::Node config = YAML::LoadFile(configurationFile);

    // get username for individual path
    const char *user = std::getenv("USER");
    std::string username(user);
    std::string basePath, dataSubPath;

    try {
        YAML::Node parentNode = config;
        if (parentNode.IsMap()) {
            YAML::iterator itLvl0 = parentNode.begin();
            YAML::Node key = itLvl0->first;
            YAML::Node value = itLvl0->second;
            if (key.as<std::string>() == "configuration") {
                if (value.IsMap()) {
                    for (auto itLvl1 = value.begin(); itLvl1 != value.end(); ++itLvl1) {
                        YAML::Node sub1Key = itLvl1->first;
                        YAML::Node sub1Value = itLvl1->second;

                        if (sub1Value.IsMap()) {
                            if (sub1Key.as<std::string>() == "mpiParameters") {
                                for (auto itLvl2 = sub1Value.begin(); itLvl2 != sub1Value.end(); ++itLvl2) {
                                    YAML::Node sub2Key = itLvl2->first;
                                    YAML::Node sub2Value = itLvl2->second;

                                    if (sub2Key.as<std::string>() == "schema") {
                                        mpiParameters.nameSchema = sub2Value.as<std::string>();
                                    } else if (sub2Key.as<std::string>() == "comInterval") {
                                        mpiParameters.comInterval = sub2Value.as<unsigned int>();
                                    } else if (sub2Key.as<std::string>() == "maxIterations") {
                                        mpiParameters.maxIterations = sub2Value.as<unsigned int>();
                                    } else if (sub2Key.as<std::string>() == "conditionThreshold") {
                                        mpiParameters.conditionThreshold = sub2Value.as<unsigned int>();
                                    }
                                }
                            } else if (sub1Key.as<std::string>() == "viParameters") {
                                for (auto itLvl2 = sub1Value.begin(); itLvl2 != sub1Value.end(); ++itLvl2) {
                                    YAML::Node sub2Key = itLvl2->first;
                                    YAML::Node sub2Value = itLvl2->second;

                                    if (sub2Key.as<std::string>() == "doAsync") {
                                        viParameters.doAsync = sub2Value.as<bool>();
                                    } else if (sub2Key.as<std::string>() == "alpha") {
                                        viParameters.alpha = sub2Value.as<float>();
                                    } else if (sub2Key.as<std::string>() == "eps") {
                                        viParameters.eps = sub2Value.as<float>();
                                    } else if (sub2Key.as<std::string>() == "maxIterations") {
                                        viParameters.maxIterations = sub2Value.as<unsigned int>();
                                    } else if (sub2Key.as<std::string>() == "conditionThreshold") {
                                        viParameters.conditionThreshold = sub2Value.as<unsigned int>();
                                    } else if (sub2Key.as<std::string>() == "numThreads") {
                                        viParameters.numThreads = sub2Value.as<unsigned int>();
                                    }
                                }
                            }
                        } else {
                            if (sub1Key.as<std::string>() == "name") {
                                logParameters.nameConfiguration = sub1Value.as<std::string>();
                            } else if (sub1Key.as<std::string>() == "resultsPath") {
                                logParameters.filePath = sub1Value.as<std::string>();
                            } else if (sub1Key.as<std::string>() == "basePath") {
                                basePath = sub1Value.as<std::string>();
                            } else if (sub1Key.as<std::string>() == "dataSubPath") {
                                dataSubPath = sub1Value.as<std::string>();
                            }
                        }
                    }
                }
            }
        }
    } catch (YAML::RepresentationException &exception) { throw std::invalid_argument("The specified configuration file is malformed: " + exception.msg); }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiParameters.worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiParameters.worldRank);

    if (mpiParameters.worldRank == 0) {
        std::cout << "You have entered " << argc << " arguments.\n";
        if (argc != 2) {
            std::cout << "You entered the wrong number of arguments: " << argc << " arguments" << std::endl;
            std::cout << "Intended use is: mpirun <options> mpi-vi <configurationpath>" << std::endl;
            for (int i = 0; i < argc; ++i) { std::cout << argv[i] << "\n"; }
            //throw std::invalid_argument("Invalid number of arguments.");
        }

        if (config["name"]) { std::cout << "Configuration name: " << config["name"] << "\n"; }
        if (config["resultsPath"]) { std::cout << "Path for results file: " << config["resultsPath"] << "\n"; }
        if (config["dataPath1"]) { std::cout << "First part of path for data set files: " << config["dataPath1"] << "\n"; }
        if (config["dataPath2"]) { std::cout << "Second part of path for data set files: " << config["dataPath2"] << "\n"; }
    }


    MpiViUtility::loadParameters(viParameters, basePath + username + dataSubPath, "params.txt");

    loadNpy<float>(data, basePath + username + dataSubPath, "P_data.npy");
    loadNpy<int>(indices, basePath + username + dataSubPath, "P_indices.npy");
    loadNpy<int>(indptr, basePath + username + dataSubPath, "P_indptr.npy");
    loadNpy<float>(jStar, basePath + username + dataSubPath, "J_star_alpha_0_99.npy");
    // yaml test


    MpiViSchema01 schema;// TODO: add to configuration file

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
        logParameters.jDiffMSE = (_j0 - _jStar).array().square().sum() / (float) _j0.size();

#ifdef VERBOSE_DEBUG
        for (int iState = 0; iState < 10; ++iState) { std::cout << iState << "\t\t>> j0: " << _j0[iState] << ";  \tjStar: " << _jStar[iState] << ";  \tpi: " << pi[iState] << std::endl; }

        for (int iState = viParameters.NS - 10; iState < (int) viParameters.NS; ++iState) { std::cout << iState << "\t\t>> j0: " << _j0[iState] << ";  \tjStar: " << _jStar[iState] << ";  \tpi: " << pi[iState] << std::endl; }
#endif
        logParameters.runtimeVi = (unsigned int) std::chrono::duration_cast<std::chrono::milliseconds>(tEndVi - tStartVi).count();
        logParameters.maxRSS = MpiViUtility::getMaxRSSUsage();

        logParameters.nameConfiguration = schema.GetName();
        auto tEnd = std::chrono::system_clock::now();
        logParameters.runtime = (unsigned int) std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tStart).count();

        MpiViUtility::saveResults(mpiParameters, logParameters);

        std::cout << "epsGlobal: " << logParameters.epsGlobal << ", max norm (J - J*): " << logParameters.jDiffMaxNorm << ", l2 norm (J - J*): " << logParameters.jDiffL2Norm << ", MSE (J - J*): " << logParameters.jDiffMSE << std::endl;
        std::cout << "converged at: " << logParameters.steps << ", runtime: " << logParameters.runtime << "ms, MpiVi took: " << logParameters.runtimeVi << "ms, with maxRSSU: " << logParameters.maxRSS << "kb" << std::endl;
    }

    MPI_Finalize();

    return 0;
}
