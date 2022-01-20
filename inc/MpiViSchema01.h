#ifndef _FirstSchema_H_
#define _FirstSchema_H_

#include "MpiViSchema.h"
#include "MpiViUtility.h"

class MpiViSchema01 : public MpiViSchema {
public:
    MpiViSchema01();
    /**
     * Execute Value iteration after schema
     * @param viParameters controls the Value Iteration parameter
     * @param mpiParameters controls the Open MPI parameter
     * @param logParameters controls the Login parameter
     */
    void ValueIteration(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters) override;
    /**
     * Function to get name of Schema
     * @return name of Schema
     */
    std::string GetName() override;
};

#endif
