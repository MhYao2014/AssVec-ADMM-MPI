//
// Created by mhyao on 2020/2/10.
//
#ifndef _LOSS_H_
#define _LOSS_H_
#include "Loss.h"
#endif
#include "matrix.h"
#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <random>

class SkipGramMpiOpenmp: public Loss {
private:
    long long vocabSize;
    int dim;
    double lossSG;
    Matrix * p2Input;
    Matrix * p2Dual;
    Matrix * p2SubProSolution;
    Matrix * p2Output;
    Matrix * p2Communicate;
    Matrix * p2Globe;
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
    void initNegAndUniTable(Dictionary * p2Dict);
    bool IfOneEpoch(FILE *p2File, int threadId, int threadNum);
    bool IfKeepTrain();
    void accumuOutput(Dictionary * p2Dict, Args * p2Args, int variId);
    void restoreOutput(Dictionary * p2Dict, Args * p2Args);
    void saveSubProSolution(int Id);
    void recordLoss(FILE * p2Record);
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