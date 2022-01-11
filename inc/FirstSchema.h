#ifndef _FirstSchema_H_
#define _FirstSchema_H_

#include "simulator.h"

class FirstSchema{
public:
    float ValueIteration (std::vector<float> &j, float *pData, int *pIndices, int *pIndptr, unsigned int pNnz, std::vector<int> &pi,
                                     float alpha, int maxF, const int nStars, int maxU,
                                     float epsThreshold, bool doAsync, int maxIteration, int comInterval);


};
#endif
