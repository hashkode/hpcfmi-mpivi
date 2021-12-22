cmake_minimum_required (VERSION 3.13)

# Additional 'find module' scipts in the subfolder 'cmake'
# Currently, FindLAPACKE is the important one. FindMKL exist for the case that you need it in another project.
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/;${CMAKE_MODULE_PATH}")

# The Linear Algebra Package (LAPACK) makes use of the operations defined in BLAS to provide the actual
# linear algebra stuff (e.g. equation solver or SVD), Numpy also uses it in the background.
#
# OpenLapack provides the implementation similar to OpenBlas, Intel MKL would be faster and has everything
# included (BLAS + LAPACK + OpenMP), but Intel is greedy and restricts it to their CPUs ...
find_package(LAPACK REQUIRED)

# Interface for Eigen between C++ and Fortran:
# Eigen, a C++ library, requires some code to communicate with the Fortran implementation of BLAS and LAPACK
# Pay attention to the E in the end.
find_package(LAPACKE REQUIRED)

# Add additional include directories for the backend:
# - eveything in our directory 'inc'
# - lib/ directory itself for sake of completeness (if we decide to place a header next to the CMakeLists.txt)
# - modules we need (mainly the optional requirements for eigen such as blas or lapack)
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/libnpy/include # TODO: hacky, introduce cmake file for cnpy?
        ${LAPACKE_INCLUDE_DIRS_DEP} # = include directories of lapacke and dependencies
)
