#include "ValueIteration.h"
#include "verbose.h"

#include "omp.h"
#include <iostream>

namespace Backend {
    float ValueIteration::valueIteration(float *j, float *pData, int *pIndices, int *pIndptr, int *pi, MpiViUtility::ViParameters &viParameters) {
        unsigned int nState = viParameters.NS;
        unsigned int nRow = viParameters.NS;
        unsigned int nCol = viParameters.NS * viParameters.max_controls;

        Eigen::Map<Eigen::VectorXf> _j(j, nState);
        Eigen::Map<Eigen::VectorXi> _pi(pi, nState);
        Eigen::Map<Eigen::SparseMatrix<float>> _p(nRow, nCol, viParameters.NS, pIndptr, pIndices, pData);

        if (viParameters.doAsync) {
            return asyncValueIteration(_j, _p, _pi, viParameters);
        } else {
            return syncValueIteration(_j, _p, _pi, viParameters);
        }
    }

    inline StateTuple ValueIteration::decodeState(unsigned int state, unsigned int nStars) {
        StateTuple stateTuple{
                .fuel = state / (nStars * nStars),
                .desStar = (state % (nStars * nStars)) / nStars,
                .curStar = (state % (nStars * nStars)) % nStars,
        };

        return stateTuple;
    }

    inline float ValueIteration::calculateStageCost(StateTuple &state, const int action) {
        float cost;
        if (state.curStar == state.desStar && action == 0) {
            cost = -100;
        } else if (state.fuel == 0) {
            cost = 100;
        } else if (action != 0) {
            cost = 5;
        } else {
            cost = 0;
        }
        return cost;
    }

    template<typename SparseMatrixType>
    inline float ValueIteration::calculateExpectedCost(Eigen::Map<Eigen::VectorXf> &j, Eigen::Map<SparseMatrixType> &p, StateTuple &stateTuple, const int state, const int action, const float alpha, const int maxU) {
        // column: current state + action
        int column = state * maxU + action;

        float expectedCost = 0;
        float cost = calculateStageCost(stateTuple, action);
        bool costUpdate = false;

        for (typename Eigen::Map<SparseMatrixType>::InnerIterator innerIterator(p, column); innerIterator; ++innerIterator) {
            // row: next state, value: transition probability
            expectedCost += innerIterator.value() * (cost + alpha * j(innerIterator.row()));
            costUpdate = true;
        }

        if (!costUpdate) { expectedCost = NAN; };

        return expectedCost;
    }

    template<typename SparseMatrixType>
    inline float ValueIteration::updateBlock(const unsigned int blockStart, const unsigned int blockSize, Eigen::Map<Eigen::VectorXf> &j, Eigen::Map<SparseMatrixType> &p, Eigen::Map<Eigen::VectorXi> pi, MpiViUtility::ViParameters &viParameters) {
        unsigned int endBlock = std::min(blockStart + blockSize, (viParameters.NS)) - 1;
        float epsLocal = -1;

        for (unsigned int state = blockStart; state <= endBlock; state++) {
            float jTmp = std::numeric_limits<float>::max();
            int uTmp = 0;
            bool costUpdate = false;
            StateTuple stateTuple = decodeState(state, viParameters.NS);

            // search minimum of j
            for (unsigned int u_i = 0; u_i < viParameters.max_controls; u_i++) {
                float J_i = calculateExpectedCost(j, p, stateTuple, state, u_i, viParameters.alpha, viParameters.max_controls);

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
    inline float ValueIteration::asyncValueIteration(Eigen::Map<Eigen::VectorXf> &j, Eigen::Map<SparseMatrixType> &p, Eigen::Map<Eigen::VectorXi> pi, MpiViUtility::ViParameters &viParameters) {
        unsigned int nState = viParameters.lastState - viParameters.firstState;
        unsigned int nBlock = 100;
        unsigned int blockSize = nState / nBlock + 1;
        float epsGlobal = -1;
        unsigned int iterations = 0;
        unsigned int conditionCount = 0;

#pragma omp parallel
#if CMAKE_BUILD_TYPE == Release
        omp_set_num_threads(viParameters.numThreads);
#endif

        while (conditionCount < viParameters.conditionThreshold && iterations < viParameters.maxIterations) {
            epsGlobal = -1;

            iterations++;

#pragma omp for schedule(guided, 1)
            for (unsigned int iBlock = 0; iBlock < nBlock; iBlock++) { epsGlobal = std::max(epsGlobal, updateBlock(viParameters.firstState + iBlock * blockSize, blockSize, j, p, pi, viParameters)); }

            if (epsGlobal < viParameters.eps) {
                conditionCount++;
            } else {
                conditionCount = 0;
            }
        }
#ifdef VERBOSE_DEBUG
        std::cout << "converged at: " << iterations << ", epsGlobal: " << epsGlobal << std::endl;
#endif
        return epsGlobal;
    }

    template<typename SparseMatrixType>
    inline float ValueIteration::syncValueIteration(Eigen::Map<Eigen::VectorXf> &j, Eigen::Map<SparseMatrixType> &p, Eigen::Map<Eigen::VectorXi> pi, MpiViUtility::ViParameters &viParameters) {
        float epsGlobal = -1;
        unsigned int iterations = 0;
        unsigned int conditionCount = 0;

        while (conditionCount < viParameters.conditionThreshold && iterations < viParameters.maxIterations) {
            Eigen::VectorXf jOld = j;

            iterations++;
            for (unsigned int state = viParameters.firstState; state <= viParameters.lastState; state++) {
                float jTmp = std::numeric_limits<float>::max();
                int uTmp = 0;
                StateTuple stateTuple = decodeState(state, viParameters.number_stars);
                bool costUpdate = false;

                // search minimum of j
                for (unsigned int u_i = 0; u_i < viParameters.max_controls; u_i++) {
                    float j_i = calculateExpectedCost(j, p, stateTuple, state, u_i, viParameters.alpha, viParameters.max_controls);

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

            if (epsGlobal < viParameters.eps) {
                conditionCount++;
            } else {
                conditionCount = 0;
            }
        }
#ifdef VERBOSE_DEBUG
        std::cout << "converged at: " << iterations << ", epsGlobal: " << epsGlobal << std::endl;
#endif
        return epsGlobal;
    }
}// namespace Backend
