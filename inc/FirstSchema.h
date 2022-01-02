#ifndef _FirstSchema_H_
#define _FirstSchema_H_

#include "simulator.h"
//#include <mpi.h>
class FirstSchema{
protected:
    template<typename SparseMatrixType>
    void AsynchronousValueIteration (Eigen::Map<Eigen::VectorXf> &j, Eigen::Map<SparseMatrixType> &p,
                                    Eigen::Map<Eigen::VectorXi> pi, float alpha, int maxF, int nStars, int maxU,
                                    float epsThreshold, int maxIteration, int commPeriode);


};
#endif
