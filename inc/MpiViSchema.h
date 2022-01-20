//
// Created by tobias on 18.01.22.
//

#ifndef MPI_VI_MPIVISCHEMA_H
#define MPI_VI_MPIVISCHEMA_H

#include "MpiViUtility.h"

class MpiViSchema {
protected:
    /**
     * Holds the name of the Schema
     */
    std::string name;

public:
    /**
     * Settings for Value Iteration
     * @param viParameters controls the Value Iteration parameter
     * @param mpiParameters controls the Open MPI parameter
     * @param logParameters controls the Login parameter
     */
    virtual void ValueIteration(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters) = 0;
    /**
     * Function to get name of Schema
     * @return name of Schema
     */
    virtual std::string GetName() = 0;
    /**
     * Calculates metrics for the results based on the cost function
     * @param j vector with the optimal cost per state
     * @param viParameters controls the Value Iteration parameter
     * @param mpiParameters controls the Open MPI parameter
     * @param logParameters controls the Login parameter
     */
    void calculateMetrics(std::vector<float> &j, MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters);
};

#endif//MPI_VI_MPIVISCHEMA_H
