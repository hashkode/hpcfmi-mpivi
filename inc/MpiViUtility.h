//
// Created by tobias on 11.01.22.
//

#ifndef MPI_VI_MPIVIUTILITY_H
#define MPI_VI_MPIVIUTILITY_H

#include <chrono>
#include <mpi.h>
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

#ifndef GIT_BRANCH
#define GIT_BRANCH "?"
#endif


class MpiViUtility {
public:
    /**
     * This type defines a struct that hold data for Value Iteration and MPI Parameter.
     */
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
    /**
     * This type defines a struct that hold Parameter for Value Iteration
     */
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
    /**
     * This type defines a struct that hold Parameter for Open MPI.
     */
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
    /**
     * This type defines a struct that hold measure values.
     */
    struct LogParameters {
        std::string filePath;
        std::string startDatetime;
        std::string target;
        unsigned int runtime;
        unsigned int runtimeVi;
        std::chrono::time_point<std::chrono::system_clock> tStart;
        std::chrono::time_point<std::chrono::system_clock> tEnd;
        unsigned long maxRSS;
        float epsGlobal;
        unsigned int steps;
        float jDiffMaxNorm;
        float jDiffL2Norm;
        float jDiffMSE;
    };
    /**
     * Function for loading Parameters
     * @param viParameters parameter for Value Iteration
     * @param path path to files
     * @param filename filename of parameters
     */
    static void loadParameters(MpiViUtility::ViParameters &viParameters, const std::string &path, const std::string &filename);
    /**
     * Decomposes input struct into individual parameters
     * @param viParameters controls the Value Iteration parameter
     * @param mpiParameters controls the Open MPI parameter
     * @param logParameters controls the Login parameter
     */
    static void parseConfiguration(ViParameters &viParameters, MpiParameters &mpiParameters, LogParameters &logParameters);
    /**
     * Load numpy data
     * @param data data to load
     * @param path path to files
     * @param filename filename of parameters
     */
    static void loadNpy(std::vector<int> &data, const std::string &path, const std::string &filename);
    /**
     * Load numpy data
     * @param data data to load
     * @param path path to files
     * @param filename filename of parameters
     */
    static void loadNpy(std::vector<float> &data, const std::string &path, const std::string &filename);
    /**
     * Output current date and time
     * @return current datetime
     */
    static std::string datetime();
    /**
     * Define header and save to File
     * @param mpiParameters controls the Open MPI parameter
     * @param logParameters controls the Login parameter
     */
    static void saveResultsToFile(const MpiViUtility::MpiParameters &mpiParameters, const MpiViUtility::LogParameters &logParameters);
    /**
     * Writes the parameter in file
     * @param filenameMeasurements generated filename of Mesurement
     * @param mpiParameters controls the Open MPI parameter
     * @param logParameters controls the Login parameter
     */
    static void appendCsv(const std::string &filenameMeasurements, const MpiViUtility::MpiParameters &mpiParameters, const MpiViUtility::LogParameters &logParameters);
    /**
     * Read out memory usage
     * @return memory usage
     */
    static long getMaxRSSUsage();
    /**
     * Write Parameter in MPI struct
     * @param viParameters controls the Value Iteration parameter>
     * @param mpiParameters controls the Open MPI parameter
     */
    static void sync_Parameters(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters);
    /**
     * Get and load configuratuion
     * @param viParameters controls the Value Iteration parameter
     * @param mpiParameters controls the Open MPI parameter
     * @param logParameters controls the Login parameter
     * @param argc argument count
     * @param argv argument vector
     */
    static void loadConfiguration(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters, const int *argc, char *argv[]);
    /**
     * Broadcast string to other processors
     * @param string  string to broadcast
     * @param mpiParameters controls the Open MPI parameter
     */
    static void bcast_string(std::string &string, MpiViUtility::MpiParameters &mpiParameters);
    /**
     * Get Results and save to file
     * @param viParameters controls the Value Iteration parameter
     * @param mpiParameters controls the Open MPI parameter
     * @param logParameters controls the Login parameter
     */
    static void saveResults(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters);
};


#endif//MPI_VI_MPIVIUTILITY_H
