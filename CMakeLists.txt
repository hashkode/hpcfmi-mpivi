cmake_minimum_required(VERSION 3.13)
project(mpi-vi)

# Enable and require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags for all configuration, -Wall enables all warnings
set(CMAKE_CXX_FLAGS "-Wall")

# Flags for debug configuration: -g activates creation of debug information in executable
set(CMAKE_CXX_FLAGS_DEBUG "-g")

# Flags for release configuration: 
# -O3:           level 3 compiler optimizations => faster code
# -march-native: compile for the native (=your computer) architecture => faster code
# -ffast-math:   allow "unsafe" match operations, i.e. those operations, where the result of a floating point operation could be unstable
#                This might be a problem for cross platform but deterministic applications (e.g. multiplayer games)
#                allow fast math => faster code
# -fopenmp:      activate openmp support (next lecture), might be enabled by default depending on your compiler
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -ffast-math -fopenmp")

# We want parallel code later-on ...
find_package(OpenMP REQUIRED)

# Open MPI Package
find_package(MPI REQUIRED)

# Add the includes for MPI
include_directories(${MPI_INCLUDE_PATH})

# Basic Linear Algebra Subprogram (BLAS): description of fastest elementary matrix / vector operations
# The Fortran realization of BLAS is provided by 'OpenBlas', and should be the fastest implementation
# without vendor lock.
#
# Intel's Math Kerner Library (MKL) is normally the best choice for even faster linear algebra code,
# but as the name suggests it is not available on AMD Machines. Also our HPC CPUs are too old ...
#
# Capital letters required here and in the following, I learned it the hard way ...
find_package(BLAS REQUIRED)

# The Linear Algebra Package (LAPACK) makes use of the operations defined in BLAS to provide the actual
# linear algebra stuff (e.g. equation solver or SVD), Numpy also uses it in the background.
#
# OpenLapack provides the implementation similar to OpenBlas, Intel MKL would be faster and has everything
# included (BLAS + LAPACK + OpenMP), but Intel is greedy and restricts it to their CPUs ...
find_package(LAPACK REQUIRED)

# All additional source files for debugging, e.g. if the debugging code is located in different files
add_executable(${PROJECT_NAME} main.cpp)

# Link against MPI Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${MPI_LIBRARIES} -lpthread)
target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp)

# To make use of the our library add a subdirectory call to build and install it
# -> no need to care about the library when using an IDE to "open" this CMakeLists.txt
add_subdirectory(lib)
add_subdirectory(inc)
add_subdirectory(src)

# adapted from: https://stackoverflow.com/a/63924044
# Get the current working branch
execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        "-DGIT_BRANCH=\"${GIT_BRANCH}\"")


# Get the latest commit hash
execute_process(
        COMMAND git describe --always --dirty --match '>barfoo<'
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        "-DGIT_COMMIT_HASH=\"${GIT_COMMIT_HASH}\"")

# Get the git user-email
execute_process(
        COMMAND git config --get user.email
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_USER_EMAIL
        OUTPUT_STRIP_TRAILING_WHITESPACE)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        "-DGIT_USER_EMAIL=\"${GIT_USER_EMAIL}\"")
