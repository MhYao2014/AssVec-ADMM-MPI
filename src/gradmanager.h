//
// Created by mhyao on 2020/3/25.
//


#ifndef ASSVEC_ADMM_MPI_GRADMANAGER_H
#define ASSVEC_ADMM_MPI_GRADMANAGER_H

#include "vector.h"
#include <random>

class GradManager {
public:
    std::minstd_rand rng;
    long long inId;
    long long inDictId;
    long long TotalToken;
    int parallelNum;
    int repeatTime;
    Vector inputGrad;
    Vector inputVec;
    Vector inputVecBackUp;
    Vector outputGrad;
    Vector outputVec;
    Vector outputVecBackUp;
    std::vector<long long> outIdCount;
    double lossSG;
    double lr;
    GradManager(int dim, int seed);
    void setLr(double lr);
};

#endif