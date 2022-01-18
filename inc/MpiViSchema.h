//
// Created by tobias on 18.01.22.
//

#ifndef MPI_VI_MPIVISCHEMA_H
#define MPI_VI_MPIVISCHEMA_H

#include "MpiViUtility.h"

class MpiViSchema {
protected:
    std::string name;

public:
    virtual void ValueIteration(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters) = 0;

    virtual std::string GetName() = 0;

    void calculateMetrics(std::vector<float> &j, std::vector<float> &jStar, MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters);
};

#endif//MPI_VI_MPIVISCHEMA_H
