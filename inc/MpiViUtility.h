//
// Created by tobias on 11.01.22.
//

#ifndef MPI_VI_MPIVIUTILITY_H
#define MPI_VI_MPIVIUTILITY_H

#include <string>
#include <vector>

//TODO: remove if not needed
#define GET_VARIABLE_NAME(Variable) (#Variable)

#ifndef GIT_COMMIT_HASH
#define GIT_COMMIT_HASH "?"
#endif

#ifndef GIT_USER_EMAIL
#define GIT_USER_EMAIL "?"
#endif


class MpiViUtility {
public:
    struct ViParameters {
        // ViParameters for the MDP
        float confusion_distance;
        unsigned int fuel_capacity;
        unsigned int max_controls;
        unsigned int number_stars;
        unsigned int NS;

        // ViParameters for P Matrix
        unsigned int cols;
        unsigned int rows;
        unsigned int data;
        unsigned int indices;
        unsigned int indptr;

        // misc.
        bool doAsync;
        float alpha;
        float eps;
        int maxIterations;
        int firstState;
        int lastState;
    };

    struct MpiParameters {
        std::string mpiViConfiguration;
        int worldSize;
        int worldRank;
        int comInterval;
        int maxIterations;
        int conditionThreshold;
    };

    struct LogParameters {
        std::string filePath;
        std::string startDatetime;
        std::string nameSchemaClass;
        unsigned int runtime;
        unsigned int runtimeVi;
        long maxRSS;
        float epsGlobal;
        int steps;
        float jDiffMaxNorm;
        float jDiffL2Norm;
        float jDiffMSE;
    };

    static void loadParameters(MpiViUtility::ViParameters &viParameters, const std::string &path, const std::string &filename);

    static std::string datetime();

    static void saveResults(const MpiViUtility::MpiParameters &mpiParameters, const MpiViUtility::LogParameters &logParameters);

    static void appendCsv(const std::string &filenameMeasurements, const MpiViUtility::MpiParameters &mpiParameters, const MpiViUtility::LogParameters &logParameters);

    static long getMaxRSSUsage();
};


#endif//MPI_VI_MPIVIUTILITY_H
