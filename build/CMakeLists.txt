cmake_minimum_required(VERSION 3.5.1)
project(AssVec_ADMM_MPI)

set(CMAKE_CXX_FLAGS "-fopenmp")
find_package(MPI)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})
set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES
        ../src/args.cpp
        ../src/dictionary.cpp
        ../src/matrix.cpp
        ../src/vector.cpp
        ../src/main.cpp
        ../src/LossFactory.cpp
        ../src/Loss.cpp
        ../src/skip-gram-mpi-openmp.cpp
        ../src/gradmanager.cpp)

set(HEADER_FILES
        ../src/args.h
        ../src/dictionary.h
        ../src/matrix.h
        ../src/vector.h
        ../src/LossFactory.h
        ../src/Loss.h
        ../src/skip-gram-mpi-openmp.h
        ../src/gradmanager.h)

add_library(AssVec-shared SHARED ${SOURCE_FILES} ${HEADER_FILES})
add_library(AssVec-static STATIC ${SOURCE_FILES} ${HEADER_FILES})
add_executable(AssVec ../src/main.cpp)
target_link_libraries(AssVec AssVec-static ${MPI_CXX_LIBRARIES})
