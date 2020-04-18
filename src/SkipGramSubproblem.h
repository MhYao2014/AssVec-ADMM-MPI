//
// Created by mhyao on 2020/4/16.
//

#ifndef ASSVEC_ADMM_MPI_SKIPGRAMSUBPROBLEM_H
#define ASSVEC_ADMM_MPI_SKIPGRAMSUBPROBLEM_H
#pragma once
#include "matrix.h"
#include "dictionary.h"
#include "Loss.h"
#include "gradmanager.h"
#include <atomic>
#include <bits/shared_ptr.h>

class SkipGramSubproblem: public Loss {
private:
    long long vocabSize;
    int dim;
    std::atomic<double> loss;
    std::shared_ptr<Vector> p2Input;
    std::shared_ptr<Vector> p2InputBackUp;
    std::shared_ptr<Matrix> p2Output;
    std::shared_ptr<Matrix> p2OutputBackUp;
    void initNegAndUniTable(Dictionary * p2Dict);
    void saveVec(FILE * p2VecFile);

public:
    SkipGramSubproblem();
    void initVariables(Dictionary* pwDict, Args* p2Args, int rank) override;
    void train(Dictionary* p2Dict, Args* p2Args, int rank) override ;
    double binaryLogistic(long long outId,
            GradManager& gradientManager,
            bool lableIsPositive,
            Args* p2Args);
    void lossEachWin(Dictionary* p2Dict,
            Args* p2Args,
            int rank,
            std::vector<long long> outIds,
            GradManager& gradientManager);
    void gradUpdate(GradManager& gradManage, Args* p2Args);
};


#endif //ASSVEC_ADMM_MPI_SKIPGRAMSUBPROBLEM_H
