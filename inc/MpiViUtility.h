//
// Created by tobias on 11.01.22.
//

#ifndef MPI_VI_MPIVIUTILITY_H
#define MPI_VI_MPIVIUTILITY_H

#include <chrono>
#include <mpi.h>
#include <string>
#include <vector>

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
     * A struct that is used for broadcasting of parameters from the configuration file loaded and parsed at rank0
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
     * A struct that holds parameter to modify Value Iteration behaviour
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
     * A struct that holds parameter to modify Open MPI behaviour
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
     * A struct to collect measurements and define the logging behaviour
     */
    struct LogParameters {
        std::string filePath;
        std::string startDatetime;
        std::string target;
        unsigned int runtime;
        unsigned int runtimeVi;
        std::chrono::time_point<std::chrono::system_clock> tStart;
        std::chrono::time_point<std::chrono::system_clock> tEnd;
        unsigned long maxRssRank0;
        unsigned long maxRssSum;
        unsigned long maxRssMin;
        unsigned long maxRssMax;
        float epsGlobal;
        unsigned int steps;
        float jDiffMaxNorm;
        float jDiffL2Norm;
        float jDiffMSE;
    };
    /**
     * Function for loading a data set parameter file from disk
     * @param viParameters Value Iteration parameters
     * @param path path to data set
     * @param filename filename of data set's parameters
     */
    static void loadParameters(MpiViUtility::ViParameters &viParameters, const std::string &path, const std::string &filename);
    /**
     * Parse a .yaml configuration for the project
     * @param viParameters Value Iteration parameters
     * @param mpiParameters Open MPI parameters
     * @param logParameters measurements and logging parameters
     */
    static void parseConfiguration(ViParameters &viParameters, MpiParameters &mpiParameters, LogParameters &logParameters);
    /**
     * Load numpy data files with integer data
     * @param data data to load
     * @param path path to files
     * @param filename filename of parameters
     */
    static void loadNpy(std::vector<int> &data, const std::string &path, const std::string &filename);
    /**
     * Load numpy data files with float data
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
     * Determine .csv header of log file and create a log file
     * @param mpiParameters Open MPI parameters
     * @param logParameters measurements and logging parameters
     */
    static void saveResultsToFile(const MpiViUtility::MpiParameters &mpiParameters, const MpiViUtility::LogParameters &logParameters);
    /**
     * Append the measurements to an existing .csv file
     * @param filenameMeasurements name of .csv file
     * @param mpiParameters Open MPI parameters
     * @param logParameters measurements and logging parameters
     */
    static void appendCsv(const std::string &filenameMeasurements, const MpiViUtility::MpiParameters &mpiParameters, const MpiViUtility::LogParameters &logParameters);
    /**
     * Read out the maxRSSU value of the process
     * @return maximum RSSU value in kilobytes during process runtime
     */
    static long getMaxRSSUsage();
    /**
     * Synchronize the parameters loaded from configuration from rank0 to all other processors
     * @param viParameters Value Iteration parameters
     * @param mpiParameters Open MPI parameter
     */
    static void syncParameters(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters);
    /**
     * Load, parse and assign configuration to parameter structs
     * @param viParameters Value Iteration parameters
     * @param mpiParameters Open MPI parameters
     * @param logParameters measurements and logging parameters
     * @param argc command line argument count
     * @param argv command line argument vector
     */
    static void loadConfiguration(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters, const int *argc, char *argv[]);
    /**
     * Broadcast a string to other processors
     * @param string string to broadcast
     * @param mpiParameters Open MPI parameters
     */
    static void bcast_string(std::string &string, MpiViUtility::MpiParameters &mpiParameters);
    /**
     * Calculate measurement metrics and store them to a file
     * @param viParameters Value Iteration parameters
     * @param mpiParameters Open MPI parameters
     * @param logParameters measurements and logging parameters
     */
    static void saveResults(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters);
};


#endif//MPI_VI_MPIVIUTILITY_H
