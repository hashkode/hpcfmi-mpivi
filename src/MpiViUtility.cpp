//
// Created by tobias on 14.01.22.
//

#include "MpiViUtility.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
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

void MpiViUtility::loadConfiguration(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters) {
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

void MpiViUtility::saveResults(const MpiViUtility::MpiParameters &mpiParameters, const MpiViUtility::LogParameters &logParameters) {
    std::filesystem::create_directories(logParameters.filePath + std::string(GIT_BRANCH) + "/");
    std::string filenameMeasurements = logParameters.filePath + std::string(GIT_BRANCH) + "/" + std::string(GIT_COMMIT_HASH) + "_" + logParameters.target + "_" + std::string(GIT_USER_EMAIL) + ".csv";

    if (!std::filesystem::exists(filenameMeasurements)) {
        std::ofstream outfileMeasurements(filenameMeasurements);
        std::string header = "datetime,";
        header += "schema,";
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
    line += mpiParameters.nameSchema + ",";
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
    struct rusage memory;
    getrusage(RUSAGE_SELF, &memory);

    if (errno == EFAULT) {
        printf("Error: EFAULT\n");
    } else if (errno == EINVAL) {
        printf("Error: EINVAL\n");
    }

    return memory.ru_maxrss;
}
