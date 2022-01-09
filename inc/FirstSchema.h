#ifndef _FirstSchema_H_
#define _FirstSchema_H_

#include "simulator.h"

class FirstSchema{
public:
    void ValueIteration (float *j, float *pData, int *pIndices, int *pIndptr, unsigned int pNnz, int *pi,
                                     float alpha, int maxF, const int nStars, int maxU,
                                     float epsThreshold, bool doAsync, int maxIteration, int commPeriode);


};
#endif
