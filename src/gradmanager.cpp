//
// Created by mhyao on 2020/2/25.
//

#include "gradmanager.h"

void GradManager::setLr(double newLr) {
    lr = newLr;
}

GradManager::GradManager(int dim, int seed):inputGrad(dim),
                                            inputVec(dim),
                                            inputVecBackUp(dim),
                                            outputGrad(dim),
                                            outputVec(dim),
                                            outputVecBackUp(dim),
                                            rng(seed),
                                            inId(0),
                                            inDictId(0),
                                            TotalToken(0),
                                            parallelNum(1),
                                            repeatTime(1),
                                            lr(0.0),
                                            lossSG(0) {}