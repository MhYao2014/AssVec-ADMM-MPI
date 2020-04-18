//
// Created by mhyao on 2020/4/16.
//

#include "SkipGramSubproblem.h"
#include "utility.h"
#include <omp.h>
#include <unistd.h>

SkipGramSubproblem::SkipGramSubproblem() : vocabSize(0),
                                            dim(0),
                                            loss(0.0),
                                            p2Input(NULL),
                                            p2Output(NULL),
                                            p2InputBackUp(NULL),
                                            p2OutputBackUp(NULL),
                                            Loss(){}

void SkipGramSubproblem::initVariables(Dictionary *p2Dict, Args *p2Args, int rank) {
    dim = p2Args->dim;
    vocabSize = p2Dict->getRealVocabSize();
    p2Input = std::make_shared<Vector>(dim);
    p2InputBackUp = std::make_shared<Vector>(dim);
    p2Input->uniform(0.01);
    p2InputBackUp->zero();
    p2InputBackUp->addVector(*p2Input,1.0);
    p2Output = std::make_shared<Matrix>(vocabSize,dim);
    p2OutputBackUp = std::make_shared<Matrix>(vocabSize,dim);
    p2Output->uniform(0.01);
    p2OutputBackUp->zero();
    p2OutputBackUp->addMatrix(*p2Output,1.0);
}

void SkipGramSubproblem::initNegAndUniTable(Dictionary* p2Dict) {
    double z = 0.0,c;
    // 遍历哈希链表词典，计算所有的词频加和
    HASHUNITID * htmp = NULL;
    for (long long i = 0 ; i < TSIZE; i++) {
        if (p2Dict->vocabHash[i] != NULL) {
            htmp = p2Dict->vocabHash[i];
            while (htmp != NULL) {
                if (htmp->id != -1) {
                    z += pow(htmp->Count, 0.5);
                }
                htmp = htmp->next;
            }
        }
    }
    // 按照公式，向负采样表里填入相应个数的id
    for (long long i = 0 ; i < TSIZE; i++) {
        if (p2Dict->vocabHash[i] != NULL) {
            htmp = p2Dict->vocabHash[i];
            while (htmp != NULL) {
                if (htmp->id != -1) {
                    c = pow(htmp->Count, 0.5);
                    // 填入对应个数的id
                    for (size_t j = 0; j < c * 10000000 / z; j++) {
                        negatives_.push_back((long long)htmp->id);
                    }
                }
                htmp = htmp->next;
            }
        }
    }
    uniform_ = std::uniform_int_distribution<size_t>(0,negatives_.size() - 1);
}

void SkipGramSubproblem::train(Dictionary *p2Dict, Args *p2Args, int rank) {
    // 初始化负采样表
    initNegAndUniTable(p2Dict);
    // 打开训练数据
    FILE* p2TrainFile;
    FILE* p2VecFile;
    // 利用openmp开多线程
    #pragma omp parallel num_threads(1) default(none), \
    shared(p2Args, p2Dict, stderr, rank,stdout), \
    private(p2TrainFile,p2VecFile)
    {
        int threadId = omp_get_thread_num(), threadNum= omp_get_num_threads();
        // 各个线程定位自己的起始阅读位置
        p2TrainFile = fopen((p2Args->vocabPath + "/0").c_str(),"r");
        fseek(p2TrainFile,threadId*size(p2TrainFile)/threadNum,SEEK_SET);
        // 开始对子问题循环subProblemEpoch次求解
        std::vector<long long> line;GradManager gradient(dim, rank);int NotReadSuccess = 0;long long tempId = 0;
        for (int i=0; i < vocabSize; i++) {
            gradient.outIdCount.push_back(0);
        }
        for (int subEpo = 0; subEpo < p2Args->subProblemEpoch; subEpo++) {
            gradient.inputVec.zero();gradient.inputVec.addVector(*p2Input,1.0);
            gradient.inputVecBackUp.zero();gradient.inputVecBackUp.addVector(*p2InputBackUp,1.0);
            while (IfOneEpoch(p2TrainFile,threadId,threadNum)) {
                // 读取文件中的一行
                p2Dict->getNumEachLine(p2TrainFile,tempId,NotReadSuccess,line);
                if (!line.empty()) {
                    // 根据该行数据，逐一进行前向反向传播，记录loss
                    lossEachWin(p2Dict,p2Args,rank,line,gradient);
                }
                // 调整步长
                double currentEpoProcess = shrinkLr(gradient,p2TrainFile,threadId,threadNum,subEpo,p2Args->subProblemEpoch,p2Args->lr);
                // 主线程输出训练信息
                if (threadId == 0) {
                    loss = gradient.lossSG / gradient.TotalToken;
                    fprintf(stderr,"\repo/total:%d/%d; Process/lr:%lf/%lf; loss:%lf", subEpo+1,p2Args->subProblemEpoch,currentEpoProcess,gradient.lr,loss.load());
                }
            }
            if (subEpo == p2Args->subProblemEpoch - 1) {
                {
                    volatile int i = 0;
                    char hostname[256];
                    gethostname(hostname, sizeof(hostname));
                    printf("PID %d on %s ready for attach\n", getpid(), hostname);
                    fflush(stdout);
                    while (0 == i)
                        sleep(5);
                }
            }
            p2InputBackUp->zero();p2InputBackUp->addVector(*p2Input,1.0);
            p2OutputBackUp->zero();p2OutputBackUp->addMatrix(*p2Output,1.0);
            fseek(p2TrainFile,threadId*size(p2TrainFile)/threadNum,SEEK_SET);
            if (threadId == 0) {
                p2VecFile = fopen((p2Args->output + std::to_string(subEpo+1)).c_str(),"w");
                saveVec(p2VecFile);
                fclose(p2VecFile);
            }
        }
        fclose(p2TrainFile);
    }
}

void SkipGramSubproblem::lossEachWin(Dictionary *p2Dict, Args *p2Args, int rank, std::vector<long long> outIds,
                                     GradManager &gradient) {
    double lossTmp;
    long long negId;
    for (auto&outId: outIds) {
        // 每个正样本清空一次inputGrad，对应更新一次inputvec的负梯度
        gradient.inputGrad.zero();
        gradient.TotalToken += 1;
        gradient.outIdCount[outId] += 1;
        lossTmp = 0.0;
        // 正样本前向传播，并返回loss，更新outvec的负梯度，积累invec的负梯度
        lossTmp += binaryLogistic(outId,gradient,true,p2Args);
        // 负样本前向传播，并返回loss，更新outvec负梯度，积累invec的负梯度
        for (int j = 0; j < p2Args->neg; j++) {
            negId = getNegative(outId,gradient.rng);
            gradient.outIdCount[negId] += 1;
            lossTmp += binaryLogistic(negId,gradient,false,p2Args);
        }
        //更新invec
        gradUpdate(gradient, p2Args);
        // 这里是多线程记录每个正样本与对应负样本的loss，会相互覆盖
        gradient.lossSG += lossTmp;
    }
}

double SkipGramSubproblem::binaryLogistic(long long outId, GradManager &gradient, bool lableIsPositive,
                                          Args *p2Args) {
    gradient.outputVec.zero();gradient.outputVecBackUp.zero();
    gradient.outputVec.addRow(*p2Output,outId,1.0);gradient.outputVecBackUp.addRow(*p2OutputBackUp,outId,1.0);
    // 计算负梯度，更新outvec的负梯度，积累invec的负梯度
    double NeiJi = gradient.inputVec.dotMul(gradient.outputVec,1.0);
    double score = sigmoid(NeiJi);
    double alpha = gradient.lr*(score - double(lableIsPositive));
    p2Output->addVectorToRow(gradient.inputVecBackUp,outId,-1.0*alpha*(1/(double)gradient.outIdCount[outId]));
    gradient.inputGrad.addVector(gradient.outputVecBackUp,-1.0*alpha*(1/(double)gradient.TotalToken));
    // 计算loss
    if (lableIsPositive) {
        return -log(score);
    } else {
        return -log(1.0 - score);
    }
}

void SkipGramSubproblem::gradUpdate(GradManager &gradient, Args *p2Args) {
    p2Input->addVector(gradient.inputGrad,1.0);
}

void SkipGramSubproblem::saveVec(FILE *p2VecFile) {
    int64_t index = 0;
    for (int64_t i = 0; i < p2Output->rows(); i++) {
        for (int64_t j = 0; j < p2Output->cols(); j++) {
            index = p2Output->cols() * i + j;
            fprintf(p2VecFile, "%f ", p2Output->data()[index]);
        }
        fprintf(p2VecFile, "\n");
    }
}