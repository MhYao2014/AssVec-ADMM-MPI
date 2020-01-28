cmake_minimum_required(VERSION 3.0.2)
project(AssVec_ADMM_MPI)

set(CMAKE_PREFIX_PATH "/usr/local/openmpi")
find_package(MPI)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})
set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES
        src/args.cpp
        src/dictionary.cpp
        src/matrix.cpp
        src/vector.cpp)

set(HEADER_FILES
        src/args.h
        src/dictionary.h
        src/matrix.h
        src/vector.h)

add_library(AssVec-static STATIC ${SOURCE_FILES} ${HEADER_FILES})
add_executable(AssVec src/main.cpp)
target_link_libraries(AssVec AssVec-static ${MPI_CXX_LIBRARIES})