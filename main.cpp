// A simple program to debug the backend code

#include <iostream>
#include <fstream>
#include <cmath>
#include <chrono>


#include "simulator.h"
#include "npy.hpp"

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

Parameters load_parameters(const std::string &path, const std::string &filename) {
    std::stringstream ss;
    ss << path << filename;

    Parameters p;

    std::ifstream ifs(ss.str());

    if (!ifs.is_open())
        throw std::runtime_error("Check the filename");

    ifs >> p.confusion_distance;
    std::cout << "Confusion " << p.confusion_distance << std::endl;

    ifs >> p.fuel_capacity;
    std::cout << "Fuel " << p.fuel_capacity << std::endl;

    ifs >> p.max_controls;
    std::cout << "MaxU " << p.max_controls << std::endl;

    ifs >> p.number_stars;
    std::cout << "N stars " << p.number_stars << std::endl;

    ifs >> p.NS;
    std::cout << "NS " << p.NS << std::endl;

    ifs >> p.cols;
    std::cout << "P Cols" << p.cols << std::endl;

    ifs >> p.rows;
    std::cout << "P Cols" << p.rows << std::endl;

    ifs >> p.data;
    std::cout << "P data Size " << p.data << std::endl;

    ifs >> p.indices;
    std::cout << "P indices Size " << p.indices << std::endl;

    ifs >> p.indptr;
    std::cout << "P indptr Size " << p.indptr << std::endl;

    return p;
}

// Loading npy files into std::vector<T>
template<typename T>
void load_npy(std::vector<T> &data, const std::string &path, const std::string &filename) {
    
    std::vector<unsigned long> shape;
    bool fortran_order;

    std::string ss = path + filename;

    std::cout << ss << std::endl;

    npy::LoadArrayFromNumpy(ss, shape, fortran_order, data);
}

int main(int argc, char *argv[]) {
    
    //Path to folder with data
    const std::string path = "../data/data_small/";

    Parameters p = load_parameters(path, "cpp/params.txt");

    std::vector<int> indices, indptr;
    std::vector<float> data, jStar;

    load_npy<float>(data, path, "P_data.npy");
    load_npy<int>(indices, path, "P_indices.npy");
    load_npy<int>(indptr, path, "P_indptr.npy");
    load_npy<float>(jStar, path, "J_star_alpha_0_99.npy");

    std::vector<float> j;
    j.reserve(p.NS);
    std::vector<int> pi;
    pi.reserve(p.NS);
    float alpha = .99;
    float eps = 1e-6;
    
        //Init Openmpi
    MPI_Init(&argc, &argv);
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // Number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // Rank of this process

    auto tStart = std::chrono::system_clock::now();
    
    FirstSchema firsttest ;
    float err=firsttest.AsynchronousValueIteration(Eigen::Map<Eigen::VectorXf> &j, Eigen::Map<SparseMatrixType> &p,
                                         Eigen::Map<Eigen::VectorXi> pi, float alpha, int maxF, int nStars, int maxU,
                                            float epsThreshold, int maxIteration, int commPeriode)

    auto tEnd = std::chrono::system_clock::now();

    Eigen::Map<Eigen::VectorXf> _j0(&j[0], p.NS);
    Eigen::Map<Eigen::VectorXf> _jStar(&jStar[0], p.NS);

    float diff = (_j0 - _jStar).template lpNorm<Eigen::Infinity>();

    std::cout << "err: " << err << ", diff:" << diff << std::endl;
    std::cout << "This took " << std::chrono::duration_cast<std::chrono::seconds>(tEnd - tStart).count() << "s"
              << std::endl;
    //*/
    MPI_Finalize(); //end openmpi
    
    return 0;
}
