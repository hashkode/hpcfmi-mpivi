//
// Created by tobias on 18.01.22.
//

#ifndef MPI_VI_MPIVISCHEMA_H
#define MPI_VI_MPIVISCHEMA_H

#include "MpiViUtility.h"

class MpiViSchema {
protected:
    /**
     * Holds the name of the class
     */
    std::string name;

public:
    /**
     * Virtual method for value iteration with Open MPI, to be overridden by inheritors
     * @param viParameters Value Iteration parameters
     * @param mpiParameters Open MPI parameters
     * @param logParameters measurements and logging parameters
     */
    virtual void ValueIteration(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters) = 0;
    /**
     * Function to get the name of the class
     * @return name of the class
     */
    virtual std::string GetName() = 0;
    /**
     * Calculates metrics for the results based on the cost function
     * @param j variable with the optimal cost determined during value iteration
     * @param viParameters Value Iteration parameters
     * @param mpiParameters Open MPI parameters
     * @param logParameters measurements and logging parameters
     */
    void calculateMetrics(std::vector<float> &j, MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters);
};

#endif//MPI_VI_MPIVISCHEMA_H
