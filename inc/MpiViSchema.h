//
// Created by tobias on 18.01.22.
//

#ifndef MPI_VI_MPIVISCHEMA_H
#define MPI_VI_MPIVISCHEMA_H

#include <vector>

#include "MpiViUtility.h"

class MpiViSchema {
protected:
    std::string name;
public:
    virtual void ValueIteration(std::vector<float> &j, float *pData, int *pIndices, int *pIndptr, std::vector<int> &pi, MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters) = 0;

    virtual std::string GetName() = 0;
};


#endif//MPI_VI_MPIVISCHEMA_H
