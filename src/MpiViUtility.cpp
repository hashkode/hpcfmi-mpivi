//
// Created by tobias on 14.01.22.
//

#include "MpiViUtility.h"

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <filesystem>
#include <iostream>
#include <mpi.h>
#include <sys/resource.h>

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
    std::string filenameMeasurements = logParameters.filePath + std::string(GIT_COMMIT_HASH) + "_" + logParameters.nameConfiguration + "_" + std::string(GIT_USER_EMAIL) + ".csv";

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
