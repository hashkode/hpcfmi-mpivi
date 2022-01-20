#ifndef _ThirdSchema_H_
#define _ThirdSchema_H_

#include "MpiViSchema.h"
#include "MpiViUtility.h"

class MpiViSchema03 : public MpiViSchema {
public:
    MpiViSchema03();

    void ValueIteration(MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters) override;

    std::string GetName() override;
};

#endif
