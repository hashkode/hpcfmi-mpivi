#ifndef _FirstSchema_H_
#define _FirstSchema_H_

#include "MpiViSchema.h"
#include "MpiViUtility.h"

class MpiViSchema01 : public MpiViSchema {
public:
    MpiViSchema01();

    void ValueIteration(std::vector<float> &j, float *pData, int *pIndices, int *pIndptr, std::vector<int> &pi, MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters) override;

    std::string GetName() override;
};

#endif
