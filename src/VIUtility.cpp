//
// Created by tobias on 14.01.22.
//

#include "VIUtility.h"

#include <stdexcept>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <fstream>

VIUtility::Parameters VIUtility::loadParameters(const std::string &path, const std::string &filename) {
    std::stringstream ss;
    ss << path << filename;

    VIUtility::Parameters p;

    std::ifstream ifs(ss.str());

    if (!ifs.is_open())
        throw std::runtime_error("Check the filename");

    ifs >> p.confusion_distance;
    ifs >> p.fuel_capacity;
    ifs >> p.max_controls;
    ifs >> p.number_stars;
    ifs >> p.NS;
    ifs >> p.cols;
    ifs >> p.rows;
    ifs >> p.data;
    ifs >> p.indices;
    ifs >> p.indptr;

    return p;
}

std::string VIUtility::datetime() {
    time_t rawtime;
    struct tm *timeinfo;
    char buffer_c[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer_c, 80, "%Y-%m-%d_%H:%M:%S", timeinfo);

    std::string buffer = std::string(buffer_c);

    return buffer;
}

void VIUtility::saveResults(const std::vector<int> &iStepVector, const std::vector<float> &durationVector,
                            const std::vector<float> &jDiffsMaxNorm, const std::vector<float> &jDiffsL2Norm,
                            const std::vector<float> &jDiffsMSE, const int comInterval, const std::string &nameSchema) {
    std::string iStepVectorName = GET_VARIABLE_NAME(iStepVector);
    std::string durationVectorName = GET_VARIABLE_NAME(durationVector);

    int nMeasurements = (int) iStepVector.size();

    std::string comIntervallString = std::to_string(comInterval);
    std::string fileFormat = ".csv";

    std::string filenameMeasurements =
            "../results/" + nameSchema + "_" + comIntervallString + "_" + datetime() + fileFormat;
    std::ofstream outfileMeasurements(filenameMeasurements);
    std::string header = "steps_total,duration_total_ms,jdiff_maxnorm,jdiff_l2norm,jdiff_mse\n";
    outfileMeasurements.write(header.c_str(), (long) header.size());

    for (int iMeasurement = 0; iMeasurement < nMeasurements; ++iMeasurement) {
        std::string line =
                std::to_string(iStepVector[iMeasurement]) + "," + std::to_string(durationVector[iMeasurement]) + "," +
                std::to_string(jDiffsMaxNorm[iMeasurement]) + "," + std::to_string(jDiffsL2Norm[iMeasurement]) + "," +
                std::to_string(jDiffsMSE[iMeasurement]) + "\n";
        outfileMeasurements.write(line.c_str(), (long) line.size());
    }
    outfileMeasurements.close();
}