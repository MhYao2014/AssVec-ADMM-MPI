//
// Created by mhyao on 2020/2/10.
//
#ifndef _LOSS_H_
#define _LOSS_H_
#include "Loss.h"
#endif

#ifndef ASSVEC_ADMM_MPI_SKIPGRAMMPIOPENMP_H
#define ASSVEC_ADMM_MPI_SKIPGRAMMPIOPENMP_H

#include "matrix.h"
#include "gradmanager.h"
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <atomic>

class SkipGramMpiOpenmp: public Loss {
private:
    long long vocabSize;
    int dim;
    std::atomic<double> lossSG;
    std::shared_ptr<Matrix> p2Input;// i_m
    std::shared_ptr<Matrix> p2InputBackUp;
    std::shared_ptr<Matrix> p2Dual;// y_m
    std::shared_ptr<Matrix> p2SubProSolution;// o_m^k
    std::shared_ptr<Matrix> p2Output;// o_m^k+1
    std::shared_ptr<Matrix> p2OutputBackUp;
    std::shared_ptr<Matrix> p2Communicate;// temp
    std::shared_ptr<Matrix> p2Globe;// o
    void initNegAndUniTable(Dictionary * p2Dict);
    void localSumOutput(Dictionary * p2Dict, Args * p2Args, int variId);
    void restoreOutput(Dictionary * p2Dict, Args * p2Args);
    void saveSubProSolution(int Id);
    void saveVec(FILE * p2VecFile);
public:
    SkipGramMpiOpenmp();
    void initVariables(Dictionary *p2Dict, Args *p2Args, int rank) override;
    void train(Dictionary *p2Dict, Args *p2Args, int rank) override;
    double binaryLogistic(long long outId,
                        GradManager &gradManager,
                        bool labelIsPositive,
                        Args * p2Args);
    void lossEachWin(Dictionary *p2Dict,
                        Args *p2Args,
                        int rank,
                        std::vector<long long> outIds,
                        GradManager &gradManager);
    void gradUpdate(GradManager &gradManager, Args * p2Args);
};

#endif