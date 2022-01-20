//
// Created by tobias on 18.01.22.
//

#include "MpiViSchema.h"

#include "Eigen/Sparse"

void MpiViSchema::calculateMetrics(std::vector<float> &j, MpiViUtility::ViParameters &viParameters, MpiViUtility::MpiParameters &mpiParameters, MpiViUtility::LogParameters &logParameters) {
    std::vector<float> jStar;
    MpiViUtility::loadNpy(jStar, mpiParameters.basePath + mpiParameters.username + mpiParameters.dataSubPath, "J_star_alpha_0_99.npy");

    Eigen::Map<Eigen::VectorXf> _j0(j.data(), viParameters.NS);
    Eigen::Map<Eigen::VectorXf> _jStar(jStar.data(), viParameters.NS);
#ifdef VERBOSE_DEBUG
    for (int iState = 0; iState < 10; ++iState) { std::cout << iState << "\t\t>> j0: " << _j0[iState] << ";  \tjStar: " << _jStar[iState] << ";  \tpi: " << pi[iState] << std::endl; }

    for (unsigned int iState = viParameters.NS - 10; iState < (int) viParameters.NS; ++iState) { std::cout << iState << "\t\t>> j0: " << _j0[iState] << ";  \tjStar: " << _jStar[iState] << ";  \tpi: " << pi[iState] << std::endl; }
#endif

    logParameters.jDiffMaxNorm = (_j0 - _jStar).template lpNorm<Eigen::Infinity>();
    logParameters.jDiffL2Norm = (_j0 - _jStar).template lpNorm<2>();
    logParameters.jDiffMSE = (_j0 - _jStar).array().square().sum() / (float) _j0.size();
}
