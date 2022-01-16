#ifndef _FirstSchema_H_
#define _FirstSchema_H_

#include "ValueIteration.h"

class MpiViSchema01 {
private:
    std::string name;

public:
    MpiViSchema01();

    std::tuple<float, int> ValueIteration(std::vector<float> &j, float *pData, int *pIndices, int *pIndptr, unsigned int pNnz, std::vector<int> &pi, float alpha, int maxF, int nStars, int maxU, float epsThreshold, bool doAsync, int maxIteration, int comInterval);

    std::string GetName();
};

#endif
