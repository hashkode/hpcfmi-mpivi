//
// Created by tobias on 11.01.22.
//

#ifndef MPI_VI_MPIVIUTILITY_H
#define MPI_VI_MPIVIUTILITY_H

#include <string>
#include <vector>

#define GET_VARIABLE_NAME(Variable) (#Variable)

#ifndef GIT_COMMIT_HASH
#define GIT_COMMIT_HASH "?"
#endif


class MpiViUtility {
public:
    struct Parameters {
        // Parameters for the MDP
        float confusion_distance;
        unsigned int fuel_capacity;
        unsigned int max_controls;
        unsigned int number_stars;
        unsigned int NS;

        // Parameters for P Matrix
        unsigned int cols;
        unsigned int rows;
        unsigned int data;
        unsigned int indices;
        unsigned int indptr;
    };

    static MpiViUtility::Parameters loadParameters(const std::string &path, const std::string &filename);

    static std::string datetime();

    static void saveResults(const std::vector<int> &iStepVector, const std::vector<float> &durationVector, const std::vector<float> &jDiffsMaxNorm, const std::vector<float> &jDiffsL2Norm, const std::vector<float> &jDiffsMSE, const std::vector<long> &maxRSSs, int comInterval, const std::string &nameSchema);

    static long getMaxRSSUsage();
};


#endif//MPI_VI_MPIVIUTILITY_H
