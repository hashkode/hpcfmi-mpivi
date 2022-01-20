#ifndef _Schema02_H_
#define _Schema02_H_

#include "MpiViSchema.h"
#include "MpiViUtility.h"

class MpiViSchema02 : public MpiViSchema {
public:
    MpiViSchema02();

    void ValueIteration(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters) override;

    std::string GetName() override;
};

#endif
