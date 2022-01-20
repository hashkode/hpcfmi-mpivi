#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#define EIGEN_USE_BLAS
#define EIGEN_USE_LAPACKE

#include "Eigen/Sparse"

#include "MpiViUtility.h"

/**
 * This namespace holds the implementation of value iteration to determine optimal cost and policy per state for a space travel problem.
 */
namespace Backend {
    /**
 * This type defines a struct to hold the decomposed state information for easier access.
 */
    typedef struct {
        unsigned int fuel;
        unsigned int desStar;
        unsigned int curStar;
    } StateTuple;

    class ValueIteration {
    public:
        /**
         *  * Run value iteration on the specified probability matrix to yield optimal cost and policy.
         * @param j j pointer to an array holding the state cost
         * @param pData pData pointer to the data property of a sparse matrix with transition data
         * @param pIndices pIndices pointer to the indices property of a sparse matrix with transition data
         * @param pIndptr pIndptr pointer to the indptr property of a sparse matrix with transition data
         * @param pi pi vector with the optimal policy per state - pointer to an array with the policy
         * @param viParameters struct for Value Iteration Parameter
         * @return epsilon global value when stop criterion was met
         */
        float valueIteration(float *j, float *pData, int *pIndices, int *pIndptr, int *pi, MpiViUtility::ViParameters &viParameters);

    private:
        /**
         * Deserialize the input state into a struct representation.
         * @param state serialized state of the space ship
         * @param nStars number of stars
         * @return deserialized state
         */
        static StateTuple decodeState(unsigned int state, unsigned int nStars);

        /**
         * Calculate the stage cost of the action at the state.
         * @param state serialized state of the space ship
         * @param action control action
         * @return stage cost
         */
        static float calculateStageCost(StateTuple &state, int action);

        /**
         * Calculate the expected cost of the action at the state.
         * @tparam SparseMatrixType template type for the sparse policy matrix
         * @param j vector with the optimal cost per state
         * @param P Eigen sparse matrix with the the transition matrix_map - Eigen sparse matrix with the transition matrix
         * @param stateTuple deserialized state of the space ship
         * @param state serialized state of the space ship
         * @param action control action
         * @param alpha decay factor for value iteration
         * @param n_stars number of stars
         * @param maxU number of possible control actions
         * @return expected cost
         */
        template<typename SparseMatrixType>
        float calculateExpectedCost(Eigen::Map<Eigen::VectorXf> &j, Eigen::Map<SparseMatrixType> &p, StateTuple &stateTuple, int state, int action, float alpha, int maxU);

        /**
         * Update optimal cost and policy for the the block.
         * @tparam SparseMatrixType template type for the sparse policy matrix
         * @param iBlock index indicating the batch of states to update
         * @param blockSize number of states per block
         * @param j vector with the optimal cost per state
         * @param p Eigen sparse matrix with the the transition matrix
         * @param pi vector with the optimal policy per state
         * @param viParameters struct for Value Iteration Parameter
         * @return  epsilon local value of the block update
         */

        template<typename SparseMatrixType>
        float updateBlock(unsigned int iBlock, unsigned int blockSize, Eigen::Map<Eigen::VectorXf> &j, Eigen::Map<SparseMatrixType> &p, Eigen::Map<Eigen::VectorXi> pi, MpiViUtility::ViParameters &viParameters);


        /**
         * Asynchronous implementation of the value iteration algorithm.
         * @tparam SparseMatrixType template type for the sparse policy matrix
         * @param j vector with the optimal cost per state
         * @param p Eigen sparse matrix with the the transition matrix
         * @param pi vector with the optimal policy per state
         * @param viParameters struct for Value Iteration Parameter
         * @return epsilon global value when stop criterion was met
         */
        template<typename SparseMatrixType>
        float asyncValueIteration(Eigen::Map<Eigen::VectorXf> &j, Eigen::Map<SparseMatrixType> &p, Eigen::Map<Eigen::VectorXi> pi, MpiViUtility::ViParameters &viParameters);

        /**
         * Synchronous implementation of the value iteration algorithm.
         * @tparam SparseMatrixType template type for the sparse policy matrix
         * @param j vector with the optimal cost per state
         * @param p Eigen sparse matrix with the the transition matrix
         * @param pi vector with the optimal policy per state
         * @param viParameters struct for Value Iteration Parameter
         * @return epsilon global value when stop criterion was met
         */
        template<typename SparseMatrixType>
        float syncValueIteration(Eigen::Map<Eigen::VectorXf> &j, Eigen::Map<SparseMatrixType> &p, Eigen::Map<Eigen::VectorXi> pi, MpiViUtility::ViParameters &viParameters);
    };
}// namespace Backend

#endif
