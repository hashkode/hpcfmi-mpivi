#ifndef MPI_VI_MPISCHEMA02_H
#define MPI_VI_MPISCHEMA02_H

#include "MpiViSchema.h"
#include "MpiViUtility.h"

class MpiViSchema02 : public MpiViSchema {
public:
    /**
     * Public constructor
     */
    MpiViSchema02();
    /**
     * Execute one cycle of value iteration
     * @param viParameters Value Iteration parameters
     * @param mpiParameters OpenMPI parameters
     * @param logParameters measurements and logging parameters
     */
    void ValueIteration(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters) override;
    /**
     * Function to get name of Schema
     * @return name of Schema
     */
    std::string GetName() override;
};

#endif//MPI_VI_MPISCHEMA02_H
