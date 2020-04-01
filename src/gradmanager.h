//
// Created by mhyao on 2020/3/25.
//

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
    Vector outputGrad;
    Vector outputVec;
    double lossSG;
    double lr;
    GradManager(int dim, int seed);
    void setLr(double lr);
};