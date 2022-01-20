#ifndef _FirstSchema_H_
#define _FirstSchema_H_

#include "MpiViSchema.h"
#include "MpiViUtility.h"

class MpiViSchema01 : public MpiViSchema {
public:
    MpiViSchema01();

    void ValueIteration(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters) override;

    std::string GetName() override;
};

#endif
