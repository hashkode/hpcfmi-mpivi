//
// Created by tobias on 11.01.22.
//

#ifndef MPI_VI_VIUTILITY_H
#define MPI_VI_VIUTILITY_H

#include <string>
#include <vector>

// verbosity setting
#define VERBOSE_INFO
//#define VERBOSE_DEBUG

#ifdef VERBOSE_DEBUG
#ifndef VERBOSE_INFO
#define VERBOSE_INFO
#endif
#endif

#define GET_VARIABLE_NAME(Variable) (#Variable)

class VIUtility {
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

    static VIUtility::Parameters loadParameters(const std::string &path, const std::string &filename);

    static std::string datetime();

    static void saveResults(const std::vector<int> &iStepVector, const std::vector<float> &durationVector, int comInterval,
                            const std::string &nameSchema);
};


#endif //MPI_VI_VIUTILITY_H
