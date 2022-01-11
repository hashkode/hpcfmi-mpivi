// A simple program to debug the backend code

#include <iostream>
#include <chrono>
#include <cstdlib>
#include <mpi.h>

#include "npy.hpp"

#include "util.h"
#include "simulator.h"
#include "FirstSchema.h"

///
/// \brief The Parameters struct
///
///  Holds the values of the text file, the order is hard coded ...
///  At least it matches to the file (might be bad for the byte alignment of this struct)
///
struct Parameters {
    // Parameters for the MDP
    float confusion_distance;
    unsigned int fuel_capacity;
    unsigned int max_controls;
    unsigned int number_stars;
    unsigned int NS;

    // Parameters for P Matrix
    unsigned int cols;
    unsigned int rows;
    unsigned int data;
    unsigned int indices;
    unsigned int indptr;
};

Parameters loadParameters(const std::string &path, const std::string &filename) {
    std::stringstream ss;
    ss << path << filename;

    Parameters p;

    std::ifstream ifs(ss.str());

    if (!ifs.is_open())
        throw std::runtime_error("Check the filename");

    ifs >> p.confusion_distance;
    ifs >> p.fuel_capacity;
    ifs >> p.max_controls;
    ifs >> p.number_stars;
    ifs >> p.NS;
    ifs >> p.cols;
    ifs >> p.rows;
    ifs >> p.data;
    ifs >> p.indices;
    ifs >> p.indptr;

    return p;
}

// Loading npy files into std::vector<T>
template<typename T>
void loadNpy(std::vector<T> &data, const std::string &path, const std::string &filename) {

    std::vector<unsigned long> shape;
    bool fortranOrder;

    std::string ss = path + filename;

#ifdef VERBOSE_DEBUG
    // std::cout << ss << std::endl;
#endif

    npy::LoadArrayFromNumpy(ss, shape, fortranOrder, data);
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int worldSize, worldRank;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

    // get username for individual path
    const char *user = std::getenv("USER");
    std::string username(user);
    // construct path to tmp directory of current user
    std::string path = "/var/tmp/" + username;
    // Add subpath to used data-set
    path += "/data/data_small/";

#ifdef VERBOSE_DEBUG
    std::cout << path << std::endl;
#endif

    Parameters p = loadParameters(path, "params.txt");

#ifdef VERBOSE_INFO
    if (worldRank == 0) {
        std::cout << "Confusion " << p.confusion_distance << std::endl;
        std::cout << "Fuel " << p.fuel_capacity << std::endl;
        std::cout << "MaxU " << p.max_controls << std::endl;
        std::cout << "N stars " << p.number_stars << std::endl;
        std::cout << "NS " << p.NS << std::endl;
        std::cout << "P Cols" << p.cols << std::endl;
        std::cout << "P Cols" << p.rows << std::endl;
        std::cout << "P data Size " << p.data << std::endl;
        std::cout << "P indices Size " << p.indices << std::endl;
        std::cout << "P indptr Size " << p.indptr << std::endl;
    }
#endif

    std::vector<int> indices, indptr;
    std::vector<float> data, jStar;

    loadNpy<float>(data, path, "P_data.npy");
    loadNpy<int>(indices, path, "P_indices.npy");
    loadNpy<int>(indptr, path, "P_indptr.npy");
    loadNpy<float>(jStar, path, "J_star_alpha_0_99.npy");

    std::vector<float> j;
    j.reserve(p.NS);
    std::vector<int> pi;
    pi.reserve(p.NS);
    float alpha = .99;
    float eps = 1e-6;

    auto tStart = std::chrono::system_clock::now();

    FirstSchema firstSchema;
    float epsGlobal = firstSchema.ValueIteration(j, data.data(), indices.data(), indptr.data(), p.NS, pi, alpha,
                                                 p.fuel_capacity,
                                                 p.number_stars, p.max_controls, eps, false, 150, 1);

    auto tEnd = std::chrono::system_clock::now();

    MPI_Finalize();

    if (worldRank == 0) {
        Eigen::Map<Eigen::VectorXf> _j0(j.data(), p.NS);
        Eigen::Map<Eigen::VectorXf> _jStar(jStar.data(), p.NS);

        float diff = (_j0 - _jStar).template lpNorm<Eigen::Infinity>();

#ifdef VERBOSE_DEBUG
        for (int i = 0; i < 10; ++i) {
            std::cout << i << ">> j0: " << _j0[i] << "; jStar: " << _jStar[i] << "; pi: " << pi[i] << std::endl;
        }
#endif

        std::cout << "epsGlobal: " << epsGlobal << ", max norm |J - J*|: " << diff << std::endl;
        std::cout << "This took " << std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tStart).count() << "ms"
                  << std::endl;
    }

    return 0;
}
