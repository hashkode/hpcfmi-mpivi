//
// Created by tobias on 14.01.22.
//

#include "MpiViUtility.h"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <sys/resource.h>

#include "npy.hpp"
#include "yaml-cpp/yaml.h"

#include "verbose.h"

void MpiViUtility::loadParameters(MpiViUtility::ViParameters &viParameters, const std::string &path, const std::string &filename) {
    std::stringstream ss;
    ss << path << filename;

    std::ifstream ifs(ss.str());

    if (!ifs.is_open()) throw std::runtime_error("Check the filename");

    ifs >> viParameters.confusion_distance;
    ifs >> viParameters.fuel_capacity;
    ifs >> viParameters.max_controls;
    ifs >> viParameters.number_stars;
    ifs >> viParameters.NS;
    ifs >> viParameters.cols;
    ifs >> viParameters.rows;
    ifs >> viParameters.data;
    ifs >> viParameters.indices;
    ifs >> viParameters.indptr;

#ifdef VERBOSE_INFO
    int worldRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
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
}

void MpiViUtility::parseConfiguration(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters) {
    YAML::Node config = YAML::LoadFile(mpiParameters.configurationFile);
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
                                        viParameters.numThreads = sub2Value.as<int>();
                                    }
                                }
                            }
                        } else {
                            if (sub1Key.as<std::string>() == "target") {
                                logParameters.target = sub1Value.as<std::string>();
                            } else if (sub1Key.as<std::string>() == "base") {
                                mpiParameters.base = sub1Value.as<std::string>();
                            } else if (sub1Key.as<std::string>() == "resultsPath") {
                                logParameters.filePath = sub1Value.as<std::string>();
                            } else if (sub1Key.as<std::string>() == "basePath") {
                                mpiParameters.basePath = sub1Value.as<std::string>();
                            } else if (sub1Key.as<std::string>() == "dataSubPath") {
                                mpiParameters.dataSubPath = sub1Value.as<std::string>();
                            }
                        }
                    }
                }
            }
        }
    } catch (YAML::RepresentationException &exception) { throw std::invalid_argument("The specified configuration file is malformed: " + exception.msg); }
}

void MpiViUtility::loadNpy(std::vector<int> &data, const std::string &path, const std::string &filename) {
    std::vector<unsigned long> shape;
    bool fortranOrder;

    std::string ss = path + filename;

    npy::LoadArrayFromNumpy(ss, shape, fortranOrder, data);
}

void MpiViUtility::loadNpy(std::vector<float> &data, const std::string &path, const std::string &filename) {
    std::vector<unsigned long> shape;
    bool fortranOrder;

    std::string ss = path + filename;

    npy::LoadArrayFromNumpy(ss, shape, fortranOrder, data);
}

std::string MpiViUtility::datetime() {
    time_t rawtime;
    struct tm *timeinfo;
    char buffer_c[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer_c, 80, "%Y-%m-%d_%H:%M:%S", timeinfo);

    std::string buffer = std::string(buffer_c);

    return buffer;
}

void MpiViUtility::saveResultsToFile(const MpiViUtility::MpiParameters &mpiParameters, const MpiViUtility::LogParameters &logParameters) {
    std::filesystem::create_directories(mpiParameters.basePath + mpiParameters.username + logParameters.filePath + std::string(GIT_BRANCH) + "/");
    std::string filenameMeasurements = mpiParameters.basePath + mpiParameters.username + logParameters.filePath + std::string(GIT_BRANCH) + "/" + std::string(GIT_COMMIT_HASH) + "_" + std::string(GIT_USER_EMAIL) + ".csv";

    if (!std::filesystem::exists(filenameMeasurements)) {
        std::ofstream outfileMeasurements(filenameMeasurements);
        std::string header = "datetime,";
        header += "target,";
        header += "data_set,";
        header += "schema,";
        header += "world_size,";
        header += "com_intervall,";
        header += "runtime_ms,";
        header += "runtime_vi_ms,";
        header += "rss_max_kb,";
        header += "steps_total,";
        header += "eps_global,";
        header += "jdiff_maxnorm,";
        header += "jdiff_l2norm,";
        header += "jdiff_mse\n";
        outfileMeasurements.write(header.c_str(), (long) header.size());
        outfileMeasurements.close();
    }

    appendCsv(filenameMeasurements, mpiParameters, logParameters);
}

void MpiViUtility::appendCsv(const std::string &filenameMeasurements, const MpiViUtility::MpiParameters &mpiParameters, const MpiViUtility::LogParameters &logParameters) {

    std::ofstream outfileMeasurements(filenameMeasurements, std::ios_base::app);

    std::string line = logParameters.startDatetime + ",";
    line += logParameters.target + ",";
    line += mpiParameters.dataSubPath + ",";
    line += mpiParameters.nameSchema + ",";
    line += std::to_string(mpiParameters.worldSize) + ",";
    line += std::to_string(mpiParameters.comInterval) + ",";
    line += std::to_string(logParameters.runtime) + ",";
    line += std::to_string(logParameters.runtimeVi) + ",";
    line += std::to_string(logParameters.maxRSS) + ",";
    line += std::to_string(logParameters.steps) + ",";
    line += std::to_string(logParameters.epsGlobal) + ",";
    line += std::to_string(logParameters.jDiffMaxNorm) + ",";
    line += std::to_string(logParameters.jDiffL2Norm) + ",";
    line += std::to_string(logParameters.jDiffMSE) + "\n";

    outfileMeasurements.write(line.c_str(), (long) line.size());
    outfileMeasurements.close();
}

long MpiViUtility::getMaxRSSUsage() {
    // adapted from: https://stackoverflow.com/a/1933854
    errno = 0;
    struct rusage memory {};
    getrusage(RUSAGE_SELF, &memory);

    if (errno == EFAULT) {
        printf("Error: EFAULT\n");
    } else if (errno == EINVAL) {
        printf("Error: EINVAL\n");
    }

    return memory.ru_maxrss;
}

void MpiViUtility::sync_Parameters(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters) {
    const int nitems = 19;
    int blocklengths[19] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    MPI_Datatype types[19] = {MPI_FLOAT, MPI_UNSIGNED, MPI_UNSIGNED, MPI_UNSIGNED, MPI_UNSIGNED, MPI_UNSIGNED, MPI_UNSIGNED, MPI_UNSIGNED, MPI_UNSIGNED, MPI_UNSIGNED, MPI_C_BOOL, MPI_FLOAT, MPI_FLOAT, MPI_UNSIGNED, MPI_UNSIGNED, MPI_INT, MPI_UNSIGNED, MPI_UNSIGNED, MPI_UNSIGNED};
    MPI_Datatype MPI_Parameterstruct;
    MPI_Aint offsets[19];

    offsets[0] = offsetof(MpiViUtility::MPI_Parameter_struct, confusion_distance);
    offsets[1] = offsetof(MpiViUtility::MPI_Parameter_struct, fuel_capacity);
    offsets[2] = offsetof(MpiViUtility::MPI_Parameter_struct, max_controls);
    offsets[3] = offsetof(MpiViUtility::MPI_Parameter_struct, number_stars);
    offsets[4] = offsetof(MpiViUtility::MPI_Parameter_struct, NS);
    offsets[5] = offsetof(MpiViUtility::MPI_Parameter_struct, cols);
    offsets[6] = offsetof(MpiViUtility::MPI_Parameter_struct, rows);
    offsets[7] = offsetof(MpiViUtility::MPI_Parameter_struct, data);
    offsets[8] = offsetof(MpiViUtility::MPI_Parameter_struct, indices);
    offsets[9] = offsetof(MpiViUtility::MPI_Parameter_struct, indptr);
    offsets[10] = offsetof(MpiViUtility::MPI_Parameter_struct, doAsync);
    offsets[11] = offsetof(MpiViUtility::MPI_Parameter_struct, alpha);
    offsets[12] = offsetof(MpiViUtility::MPI_Parameter_struct, eps);
    offsets[13] = offsetof(MpiViUtility::MPI_Parameter_struct, vi_maxIterations);
    offsets[14] = offsetof(MpiViUtility::MPI_Parameter_struct, vi_conditionThreshold);
    offsets[15] = offsetof(MpiViUtility::MPI_Parameter_struct, numThreads);
    offsets[16] = offsetof(MpiViUtility::MPI_Parameter_struct, comInterval);
    offsets[17] = offsetof(MpiViUtility::MPI_Parameter_struct, mpi_maxIterations);
    offsets[18] = offsetof(MpiViUtility::MPI_Parameter_struct, mpi_conditionThreshold);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_Parameterstruct);
    MPI_Type_commit(&MPI_Parameterstruct);

    MpiViUtility::MPI_Parameter_struct mpi_param_struct;
    if (mpiParameters.worldRank == 0) {
        mpi_param_struct.confusion_distance = viParameters.confusion_distance;
        mpi_param_struct.fuel_capacity = viParameters.fuel_capacity;
        mpi_param_struct.max_controls = viParameters.max_controls;
        mpi_param_struct.number_stars = viParameters.number_stars;
        mpi_param_struct.NS = viParameters.NS;
        mpi_param_struct.cols = viParameters.cols;
        mpi_param_struct.rows = viParameters.rows;
        mpi_param_struct.data = viParameters.data;
        mpi_param_struct.indices = viParameters.indices;
        mpi_param_struct.indptr = viParameters.indptr;
        mpi_param_struct.doAsync = viParameters.doAsync;
        mpi_param_struct.alpha = viParameters.alpha;
        mpi_param_struct.eps = viParameters.eps;
        mpi_param_struct.vi_maxIterations = viParameters.maxIterations;
        mpi_param_struct.vi_conditionThreshold = viParameters.conditionThreshold;
        mpi_param_struct.numThreads = viParameters.numThreads;
        mpi_param_struct.comInterval = mpiParameters.comInterval;
        mpi_param_struct.mpi_maxIterations = mpiParameters.maxIterations;
        mpi_param_struct.mpi_conditionThreshold = mpiParameters.conditionThreshold;
    }

    MPI_Bcast(&mpi_param_struct, 1, MPI_Parameterstruct, 0, MPI_COMM_WORLD);

    if (mpiParameters.worldRank != 0) {
        viParameters.confusion_distance = mpi_param_struct.confusion_distance;
        viParameters.fuel_capacity = mpi_param_struct.fuel_capacity;
        viParameters.max_controls = mpi_param_struct.max_controls;
        viParameters.number_stars = mpi_param_struct.number_stars;
        viParameters.NS = mpi_param_struct.NS;
        viParameters.cols = mpi_param_struct.cols;
        viParameters.rows = mpi_param_struct.rows;
        viParameters.data = mpi_param_struct.data;
        viParameters.indices = mpi_param_struct.indices;
        viParameters.indptr = mpi_param_struct.indptr;
        viParameters.doAsync = mpi_param_struct.doAsync;
        viParameters.alpha = mpi_param_struct.alpha;
        viParameters.eps = mpi_param_struct.eps;
        viParameters.maxIterations = mpi_param_struct.vi_maxIterations;
        viParameters.conditionThreshold = mpi_param_struct.vi_conditionThreshold;
        viParameters.numThreads = mpi_param_struct.numThreads;
        mpiParameters.comInterval = mpi_param_struct.comInterval;
        mpiParameters.maxIterations = mpi_param_struct.mpi_maxIterations;
        mpiParameters.conditionThreshold = mpi_param_struct.mpi_conditionThreshold;
    }
    MPI_Type_free(&MPI_Parameterstruct);

    bcast_string(mpiParameters.nameSchema, mpiParameters);
    bcast_string(mpiParameters.username, mpiParameters);
    bcast_string(mpiParameters.basePath, mpiParameters);
    bcast_string(mpiParameters.dataSubPath, mpiParameters);
    bcast_string(mpiParameters.configurationFile, mpiParameters);
}

void MpiViUtility::bcast_string(std::string &string, MpiViUtility::MpiParameters &mpiParameters) {
    int count = string.size();
    MPI_Bcast(&count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (mpiParameters.worldRank != 0) string.resize(count);
    MPI_Bcast((void *) (string.data()), count, MPI_CHAR, 0, MPI_COMM_WORLD);
}

void MpiViUtility::loadConfiguration(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters, const int *argc, char *argv[]) {
    bool useDefault = false;

    // commandline argument checks
    if (mpiParameters.worldRank == 0) {
        try {
            if (*argc == 2) {
                if (argv[1] == std::string("-d")) {
                    useDefault = true;
                } else if (!std::filesystem::exists(argv[1])) {
                    std::cout << "The specified configuration file does not exist." << std::endl;
                    throw std::invalid_argument("Invalid number of arguments.");
                }
            } else if (*argc != 2 || (argv[1] != std::string("-d") && !std::filesystem::exists(argv[1]))) {
                std::cout << "You entered wrong arguments." << std::endl;
                throw std::invalid_argument("Invalid number of arguments.");
            }
        } catch (std::invalid_argument &error) {
            std::cout << ">>> Intended use is:\n1, default:\tmpirun <options> mpi-vi -d" << std::endl;
            std::cout << "2, configured:\tmpirun <options> mpi-vi <configurationpath>" << std::endl;
            std::cout << ">>> You entered the following arguments:" << std::endl;

            for (int i = 0; i < *argc; ++i) { std::cout << std::to_string(i) + ": " << argv[i] << std::endl; }

            useDefault = true;
        }

        // get username for individual path
        const char *user = std::getenv("USER");
        mpiParameters.username = user;

        if (useDefault) {
            // default configuration file
            mpiParameters.configurationFile = "../automation/jobs/default.yaml";
            std::cout << ">>> Proceeding with default configuration file: " << mpiParameters.configurationFile << std::endl;
        } else {
            mpiParameters.configurationFile = argv[1];
        }

        MpiViUtility::parseConfiguration(viParameters, mpiParameters, logParameters);
        // TODO: replace poor man's configuration inheritance with recursive implementation
        if (mpiParameters.base != "root") {
            std::string configLvl2 = mpiParameters.configurationFile;
            mpiParameters.configurationFile = "../automation/jobs/" + mpiParameters.base;
            MpiViUtility::parseConfiguration(viParameters, mpiParameters, logParameters);

            if (mpiParameters.base != "root") {
                std::string configLvl1 = mpiParameters.configurationFile;
                mpiParameters.configurationFile = "../automation/jobs/" + mpiParameters.base;
                MpiViUtility::parseConfiguration(viParameters, mpiParameters, logParameters);

                mpiParameters.configurationFile = configLvl1;
                MpiViUtility::parseConfiguration(viParameters, mpiParameters, logParameters);
            }

            mpiParameters.configurationFile = configLvl2;
            MpiViUtility::parseConfiguration(viParameters, mpiParameters, logParameters);
        }
        MpiViUtility::loadParameters(viParameters, mpiParameters.basePath + mpiParameters.username + mpiParameters.dataSubPath, "params.txt");
    }
}

void MpiViUtility::saveResults(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters) {
    // measurement and logging
    if (mpiParameters.worldRank == 0) {
        logParameters.maxRSS = MpiViUtility::getMaxRSSUsage();

        logParameters.tEnd = std::chrono::system_clock::now();
        logParameters.runtime = (unsigned int) std::chrono::duration_cast<std::chrono::milliseconds>(logParameters.tEnd - logParameters.tStart).count();

        MpiViUtility::saveResultsToFile(mpiParameters, logParameters);

        std::cout << "epsGlobal: " << logParameters.epsGlobal << ", max norm (J - J*): " << logParameters.jDiffMaxNorm << ", l2 norm (J - J*): " << logParameters.jDiffL2Norm << ", MSE (J - J*): " << logParameters.jDiffMSE << std::endl;
        std::cout << "converged at: " << logParameters.steps << ", runtime: " << logParameters.runtime << "ms, MpiVi took: " << logParameters.runtimeVi << "ms, with maxRSSU: " << logParameters.maxRSS << "kb" << std::endl;
    }
}
