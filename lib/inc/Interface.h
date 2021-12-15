#include "Simulator.h"

/**
 * This header defines the interface of the C++ value iteration solution for use in other environments, e.g. the cffi framework in Python.
 */
extern "C"
{
/**
 * Run value iteration on the specified probability matrix to yield optimal cost and policy.
 * @param j pointer to an array holding the state cost
 * @param pData pointer to the data property of a sparse matrix with transition data
 * @param pIndices pointer to the indices property of a sparse matrix with transition data
 * @param pIndptr pointer to the indptr property of a sparse matrix with transition data
 * @param pNnz the (estimated) number of nonzero elements in the sparse matrix
 * @param pi vector with the optimal policy per state - pointer to an array with the policy
 * @param alpha decay factor for value iteration
 * @param maxF maximum fuel state of the spaceship
 * @param nStars number of stars
 * @param maxU number of possible control actions
 * @param epsThreshold epsilon threshold stop criterion
 * @param doAsync switch to enable asynchronous value iteration
 * @return epsilon global value when stop criterion was met
 */
extern float
cffi_value_iteration(float *j, float *pData, int *pIndices, int *pIndptr, unsigned int pNnz, int *pi, float alpha,
                     int maxF, int nStars, int maxU, float eps, bool doAsync) {
    return Backend::valueIteration(j, pData, pIndices, pIndptr, pNnz, pi, alpha, maxF, nStars, maxU, eps,
                                   doAsync);
}
}
