cmake_minimum_required(VERSION 3.13)

# add all files in this directory with .cpp file ending to the current mpi-vi target
file(GLOB SRC_FILES *.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${SRC_FILES})
