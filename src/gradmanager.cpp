//
// Created by mhyao on 2020/2/25.
//

#include "gradmanager.h"

GradManager::GradManager(int dim, int seed):inputGrad(dim),
                                            inputVec(dim),
                                            outputGrad(dim),
                                            outputVec(dim),
                                            rng(seed),
                                            inId(0),
                                            lossSG(0) {}