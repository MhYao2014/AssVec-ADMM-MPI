//
// Created by mhyao on 2020/3/25.
//

#include "vector.h"
#include <random>

class GradManager {
public:
    std::minstd_rand rng;
    long long inId;
    Vector inputGrad;
    Vector inputVec;
    Vector outputGrad;
    Vector outputVec;
    double lossSG;
    GradManager(int dim, int seed);
};