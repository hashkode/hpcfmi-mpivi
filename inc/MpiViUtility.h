//
// Created by tobias on 11.01.22.
//

#ifndef MPI_VI_MPIVIUTILITY_H
#define MPI_VI_MPIVIUTILITY_H

#include <string>
#include <vector>
#include <mpi.h>

//TODO: remove if not needed
#define GET_VARIABLE_NAME(Variable) (#Variable)

#ifndef GIT_COMMIT_HASH
#define GIT_COMMIT_HASH "?"
#endif

#ifndef GIT_USER_EMAIL
#define GIT_USER_EMAIL "?"
#endif

#ifndef GIT_BRANCH
#define GIT_BRANCH "?"
#endif


class MpiViUtility {
public:

    struct MPI_Parameter_struct {
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
        unsigned int vi_maxIterations;
        unsigned int vi_conditionThreshold;
        int numThreads;

        // mpi
        unsigned int comInterval;
        unsigned int mpi_maxIterations;
        unsigned int mpi_conditionThreshold;
    };

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
        unsigned int maxIterations;
        unsigned int firstState;
        unsigned int lastState;
        unsigned int conditionThreshold;
        int numThreads;
    };

    struct MpiParameters {
        std::string nameSchema;
        int worldSize;
        int worldRank;
        unsigned int comInterval;
        unsigned int maxIterations;
        unsigned int conditionThreshold;
        std::string username;
        std::string basePath;
        std::string dataSubPath;
        std::string configurationFile;
        std::string base;
    };

    struct LogParameters {
        std::string filePath;
        std::string startDatetime;
        std::string target;
        unsigned int runtime;
        unsigned int runtimeVi;
        unsigned long maxRSS;
        float epsGlobal;
        unsigned int steps;
        float jDiffMaxNorm;
        float jDiffL2Norm;
        float jDiffMSE;
    };

    static void loadParameters(MpiViUtility::ViParameters &viParameters, const std::string &path, const std::string &filename);

    static void loadConfiguration(ViParameters &viParameters, MpiParameters &mpiParameters, LogParameters &logParameters);

    static void loadNpy(std::vector<int> &data, const std::string &path, const std::string &filename);

    static void loadNpy(std::vector<float> &data, const std::string &path, const std::string &filename);

    static std::string datetime();

    static void saveResults(const MpiViUtility::MpiParameters &mpiParameters, const MpiViUtility::LogParameters &logParameters);

    static void appendCsv(const std::string &filenameMeasurements, const MpiViUtility::MpiParameters &mpiParameters, const MpiViUtility::LogParameters &logParameters);

    static long getMaxRSSUsage();

    static void sync_Parameters(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters);

    static void bcast_string(std::string &string, MpiViUtility::MpiParameters &mpiParameters);
};


#endif//MPI_VI_MPIVIUTILITY_H
