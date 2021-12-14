// A simple program to debug the backend code

#include <iostream>
#include <fstream>
#include <cmath>
#include <chrono>

#include "Simulator.h"

// start of sample code from HPCfMI lecture
template<typename T>
void
load_data(std::vector<T> &data, const std::string &path, const std::string &filename, const unsigned int num_elements) {
    std::stringstream ss;
    ss << path << filename;
    std::ifstream ifs(ss.str(), std::ios::binary);

    if (!ifs.is_open())
        throw std::runtime_error("Check the filename");

    // Copy file into memory as byte sequence (pay attention to endian)
    std::vector<char> buffer((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

    // Reinterpet the byte sequence based on template data type
    T *buffer_casted = reinterpret_cast<T *>(buffer.data());

    // Copy the elements, afaik there is no other (simpler) way to use the bytes directly
    data.resize(num_elements);
    std::copy(buffer_casted, buffer_casted + num_elements, data.data());
}

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
// end of sample code from HPCfMI lecture

int main(int argc, char *argv[]) {
    const std::string path = "../../../data/data_normal/cpp/";

    Parameters p = load_parameters(path, "params.txt");

    std::vector<int> indices, indptr;
    std::vector<float> data, jStar;

    load_data<float>(data, path, "P_data.bin", p.data);
    load_data<int>(indices, path, "P_indices.bin", p.indices);
    load_data<int>(indptr, path, "P_indptr.bin", p.indptr);
    load_data<float>(jStar, path, "P_indptr.bin", p.NS);

    std::vector<float> j;
    j.reserve(p.NS);
    std::vector<int> pi;
    pi.reserve(p.NS);
    float alpha = .99;
    float eps = 1e-6;

    auto tStart = std::chrono::system_clock::now();

    float err = Backend::valueIteration(&j[0], &data[0], &indices[0], &indptr[0], p.NS, &pi[0], alpha,
                                        p.fuel_capacity, p.number_stars, p.max_controls, eps, true);

    auto tEnd = std::chrono::system_clock::now();

    Eigen::Map<Eigen::VectorXf> _j0(&j[0], p.NS);
    Eigen::Map<Eigen::VectorXf> _jStar(&jStar[0], p.NS);

    float diff = (_j0 - _jStar).template lpNorm<Eigen::Infinity>();

    std::cout << "err: " << err << ", diff:" << diff << std::endl;
    std::cout << "This took " << std::chrono::duration_cast<std::chrono::seconds>(tEnd - tStart).count() << "s"
              << std::endl;

    return 0;
}
