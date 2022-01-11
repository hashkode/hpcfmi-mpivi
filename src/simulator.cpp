#include "util.h"
#include "simulator.h"

#include <iostream>

namespace Backend {
    float
    ValueIteration::valueIteration(float *j, float *pData, int *pIndices, int *pIndptr, unsigned int pNnz, int *pi,
                                   const float alpha, const int maxF, const int nStars, const int maxU,
                                   const float epsThreshold, const bool doAsync, const int nIteration,
                                   const int firstState,
                                   const int lastState) {
        int nState = maxF * nStars * nStars;
        int nRow = nState;
        int nCol = nState * maxU;

        Eigen::Map<Eigen::VectorXf> _j(j, nState);
        Eigen::Map<Eigen::VectorXi> _pi(pi, nState);
        Eigen::Map<Eigen::SparseMatrix<float>> _p(nRow, nCol, pNnz, pIndptr, pIndices, pData);

        if (doAsync) {
            return asyncValueIteration(_j, _p, _pi, alpha, maxF, nStars, maxU, epsThreshold, nIteration, firstState,
                                       lastState);
        } else {
            return syncValueIteration(_j, _p, _pi, alpha, maxF, nStars, maxU, epsThreshold, nIteration, firstState,
                                      lastState);
        }

    }

    inline StateTuple ValueIteration::decodeState(int state, int nStars) {
        StateTuple stateTuple{
                .fuel = (int) (state / (nStars * nStars)),
                .desStar = (int) ((state % (nStars * nStars)) / nStars),
                .curStar = (state % (nStars * nStars)) % nStars,
        };

        return stateTuple;
    }

    inline float ValueIteration::calculateStageCost(StateTuple &state, const int action) {
        float cost;
        if (state.curStar == state.desStar && action == 0) { cost = -100; }
        else if (state.fuel == 0) { cost = 100; }
        else if (action != 0) { cost = 5; }
        else { cost = 0; }
        return cost;
    }

    template<typename SparseMatrixType>
    inline float
    ValueIteration::calculateExpectedCost(Eigen::Map<Eigen::VectorXf> &j, Eigen::Map<SparseMatrixType> &p,
                                          StateTuple &stateTuple,
                                          const int state,
                                          const int action, const float alpha, const int maxU) {
        // column: current state + action
        int column = state * maxU + action;

        float expectedCost = 0;
        float cost = calculateStageCost(stateTuple, action);
        bool costUpdate = false;

        for (typename Eigen::Map<SparseMatrixType>::InnerIterator innerIterator(p,
                                                                                column); innerIterator; ++innerIterator) {
            // row: next state, value: transition probability
            expectedCost += innerIterator.value() * (cost + alpha * j(innerIterator.row()));
            costUpdate = true;
        }

        if (!costUpdate) { expectedCost = NAN; };

        return expectedCost;
    }

    template<typename SparseMatrixType>
    inline float
    ValueIteration::updateBlock(const int blockStart, const int blockSize, Eigen::Map<Eigen::VectorXf> &j,
                                Eigen::Map<SparseMatrixType> &p,
                                Eigen::Map<Eigen::VectorXi> pi, const float alpha, const int maxF, const int nStars,
                                const int maxU) {
        int endBlock = std::min(blockStart + blockSize, (maxF * nStars * nStars)) - 1;
        float epsLocal = -1;

        for (int state = blockStart; state <= endBlock; state++) {
            float jTmp = std::numeric_limits<float>::max();
            int uTmp = 0;
            bool costUpdate = false;
            StateTuple stateTuple = decodeState(state, nStars);

            // search minimum of j
            for (int u_i = 0; u_i < maxU; u_i++) {
                float J_i = calculateExpectedCost(j, p, stateTuple, state, u_i, alpha, maxU);

                if (J_i < jTmp && J_i != NAN) {
                    jTmp = J_i;
                    uTmp = u_i;
                    costUpdate = true;
                }
            }

            if (costUpdate) {
                epsLocal = std::max(epsLocal, std::abs(j(state) - jTmp));
                pi(state) = uTmp;
                j(state) = jTmp;
            }

        }
        return epsLocal;
    }

    template<typename SparseMatrixType>
    inline float ValueIteration::asyncValueIteration(Eigen::Map<Eigen::VectorXf> &j, Eigen::Map<SparseMatrixType> &p,
                                                     Eigen::Map<Eigen::VectorXi> pi, const float alpha, const int maxF,
                                                     const int nStars, const int maxU, const float epsThreshold,
                                                     const int nIteration,
                                                     const int firstState, const int lastState) {
        int nState = lastState - firstState;
        int nBlock = 100;
        int blockSize = nState / nBlock + 1;
        float epsGlobal = -1;
        int iterations = 0;
        int conditionCount = 0;
        int conditionThreshold = 5;

        while (conditionCount < conditionThreshold && iterations < nIteration) {
            epsGlobal = -1;

            iterations++;

#pragma omp parallel
#pragma omp for schedule(guided, 1)
            for (int iBlock = 0; iBlock < nBlock; iBlock++) {
                epsGlobal = std::max(epsGlobal,
                                     updateBlock(firstState + iBlock * blockSize, blockSize, j, p, pi, alpha, maxF,
                                                 nStars, maxU));
            }

            if (epsGlobal < epsThreshold) { conditionCount++; }
            else { conditionCount = 0; }
        }
#ifdef VERBOSE_DEBUG
        std::cout << "converged at: " << iterations << ", epsGlobal: " << epsGlobal << std::endl;
#endif
        return epsGlobal;
    }

    template<typename SparseMatrixType>
    inline float ValueIteration::syncValueIteration(Eigen::Map<Eigen::VectorXf> &j, Eigen::Map<SparseMatrixType> &p,
                                                    Eigen::Map<Eigen::VectorXi> pi, const float alpha, const int maxF,
                                                    const int nStars, const int maxU, const float epsThreshold,
                                                    const int nIteration,
                                                    const int firstState, const int lastState) {
        float epsGlobal = -1;
        int iterations = 0;
        int conditionCount = 0;
        int conditionThreshold = 5;

        while (conditionCount < conditionThreshold && iterations < nIteration) {
            Eigen::VectorXf jOld = j;

            iterations++;
            for (int state = firstState; state <= lastState; state++) {
                float jTmp = std::numeric_limits<float>::max();
                int uTmp = 0;
                StateTuple stateTuple = decodeState(state, nStars);
                bool costUpdate = false;

                // search minimum of j
                for (int u_i = 0; u_i < maxU; u_i++) {
                    float j_i = calculateExpectedCost(j, p, stateTuple, state, u_i, alpha, maxU);

                    if (j_i < jTmp && j_i != NAN) {
                        jTmp = j_i;
                        uTmp = u_i;
                        costUpdate = true;
                    }
                }

                if (costUpdate) {
                    pi(state) = uTmp;
                    j(state) = jTmp;
                }
            }

            epsGlobal = (j - jOld).template lpNorm<Eigen::Infinity>();

            if (epsGlobal < epsThreshold) { conditionCount++; }
            else { conditionCount = 0; }
        }
#ifdef VERBOSE_DEBUG
        std::cout << "converged at: " << iterations << ", epsGlobal: " << epsGlobal << std::endl;
#endif
        return epsGlobal;
    }
}
