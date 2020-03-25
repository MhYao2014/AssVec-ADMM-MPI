//
// Created by mhyao on 2020/2/10.
//
#pragma once
#include "skip-gram-mpi-openmp.h"
#include "matrix.h"
#include "dictionary.h"
#include "args.h"

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

SkipGramMpiOpenmp::SkipGramMpiOpenmp(): dim(100),
                                        vocabSize(10000),
                                        lossSG(0.0),
                                        p2Input(NULL),
                                        p2Dual(NULL),
                                        p2Globe(NULL),
                                        p2Output(NULL),
                                        p2SubProSolution(NULL),
                                        p2Communicate(NULL),
                                        Loss(){}

void SkipGramMpiOpenmp::initVariables(Dictionary *p2Dict, Args *p2Args, int rank) {
    // 设定矩阵行和列的大小
    vocabSize = p2Dict->getRealVocabSize();
    dim = p2Args->dim;
    // 各个进程为自己的模型申请内存空间
    p2Input = new Matrix(p2Dict->groups[rank].FileNum, dim);
    p2Input->uniform(0.1);
    p2Dual = new Matrix(p2Dict->groups[rank].FileNum,dim*vocabSize);
    p2Dual->uniform(0.1);
    p2SubProSolution = new Matrix(vocabSize, dim);
    p2Output = new Matrix(vocabSize,dim);
    p2Communicate = new Matrix(vocabSize, dim);
    p2Globe = new Matrix(vocabSize, dim);
    if (rank == 0) {
        // 主进程随机初始化output vec参数,
        p2Communicate->uniform(0.1);
        // 开始主进程通信：将主进程的通信数组分配到各个slave进程中的通信数组中去
        MPI_Bcast(p2Communicate->data(), vocabSize*dim, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        // 将通信数组拷贝到缓存数组和工作数组中去。
        p2Globe->zero();
        p2Globe->addMatrix(*p2Communicate, 1);
        p2Output->zero();
        p2Output->addMatrix(*p2Communicate, 1);
        // 将communicate数组清零，方便后面做局部平均
        p2Communicate->zero();
    }
    if (rank != 0) {
        // slave进程开始等待主进程发送消息,统一采用阻塞通信
        MPI_Bcast(p2Communicate->data(),vocabSize*dim,MPI_DOUBLE,0,MPI_COMM_WORLD);
        // 将通信数组拷贝到缓存数组和工作数组中去。
        p2Globe->zero();
        p2Globe->addMatrix(*p2Communicate, 1);
        p2Output->zero();
        p2Output->addMatrix(*p2Communicate, 1);
        // 将communicate数组清零，方便后面做局部平均
        p2Communicate->zero();
    }
    // 阻断直到所有slave进程都收到了数据并初始化成功
    MPI_Barrier(MPI_COMM_WORLD);
}

void SkipGramMpiOpenmp::train(Dictionary *p2Dict, Args *p2Args, int rank) {
    initNegAndUniTable(p2Dict); // 初始化负采样表。该表位于字典内。
    FILE *p2Record;
    p2Record = fopen(("rank" + std::to_string(rank)).c_str(),"w");
    // 执行ADMM迭代
    long long tempFileName; // private
    FILE *p2File; // private
    std::vector<long long> tempWinSamp; // firstprivate
    int NotReadSuccess = 0; // firstprivate
    long long tempId = 0; // firstprivate。这个tempId是getNumEachLine要求的一个零时变量。没设计好这一块。
    for (int epo = 0; epo < p2Args->epoch; epo++) {// 总的ADMM迭代轮数
        // 每个进程遍历自己分组中的各个文件,
        for (int i = 0; i < p2Dict->groups[rank].FileNum; i++) {
            #pragma omp parallel default(none), \
            shared(i, p2Dict, p2Args, rank), \
            firstprivate(tempId, NotReadSuccess, tempWinSamp), \
            private(tempFileName, p2File)
            {
                GradManager gradManager = GradManager(p2Args->dim, rank);
                tempFileName = (long long) i; //根据进程rank找到自己的文件分组，然后遍历其中各个文件。这里后期需要补充一个映射关系
                gradManager.inputVec.zero(); // 取出对应的input向量
                p2Input->addRowToVector(gradManager.inputVec, tempFileName, 1.0);
                gradManager.inId = tempFileName;
                // 打开tempFileName词对应的训练数据文件;
                p2File = fopen(std::to_string(tempFileName).c_str(), "r");
                if (p2File == NULL) {throw "Failed to open training file";} // 判断是否打开成功
                //将各个线程定位到各自的文件块位置
                int threadId = omp_get_thread_num(), threadNum= omp_get_num_threads();
                std::fseek(p2File, threadId * size(p2File) / threadNum, SEEK_SET);
                // 依靠多线程并行(openmp)实现Hogwild!算法。整体上，进程的各个线程实际是在合作解决同一个子问题(某个训练文件)。采用的是迭代固定次数,早停求解方法。
                for (int j = 0; j < p2Args->subProblemEpoch; j++) {
                    while (IfOneEpoch(p2File,threadId,threadNum)) {
                        // 逐行读取训练数据,一行是tempFileName对应词为中心词时，一个词窗中的周围词的id。最后将这些id存入tempWinSamp向量中保存。
                        p2Dict->getNumEachLine(p2File, tempId, NotReadSuccess, tempWinSamp);
                        if (!tempWinSamp.empty()) {
                            // 计算loss,计算并更新所有梯度
                            SkipGramMpiOpenmp::lossEachWin(p2Dict, p2Args, rank, tempWinSamp, gradManager);
                        }
                    }
                    // 一个线程的一个epoch训练完了，重置该线程的文本指针，进入下一轮训练
                    std::fseek(p2File, threadId * size(p2File) / threadNum, SEEK_SET);
                }
                fclose(p2File);// 每求解完一个子问题就关闭其对应文件
            };
            accumuOutput(p2Dict, p2Args, i); // 每求解完一个子问题,就将当前参数累加到通信数组中去
            saveSubProSolution(i); // 把每个子问题的output的解保存下来
            restoreOutput(p2Dict, p2Args); // 将缓存数组中当前ADMM轮数的原始参数复制到工作数组中
            fclose(p2File);
        }
        // 所有进程的所有子问题求解完毕,同主进程同步通信(缓存数组),平均参数
        MPI_Allreduce(p2Communicate->data(),p2Globe->data(),vocabSize*dim,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);
        p2Globe->scalerMul(1.0 / vocabSize);
        p2Communicate->zero();
        // 实施梯度上升算法
        for (int i = 0; i < p2Dict->groups[rank].FileNum; i++) {
            p2SubProSolution->addMat2Row(*p2Globe, i, -1);
            p2Dual->addRow2Row(*p2SubProSolution,i,i,p2Args->rhoOut);
        }
        // 主进程判断残差是否收敛,或者是否达到最大迭代次数
        if (IfKeepTrain()) {
            // 记录训练日志
            recordLoss(p2Record);
        } else {
            break;
        }
    }
    // ADMM算法结束,
    // 主进程保存相关数据,和词向量文件
    if (rank == 0) {
        FILE * p2VecFile = fopen(p2Args->output.c_str(),"w");
        saveVec(p2VecFile);
        fclose(p2VecFile);
    }
    fclose(p2Record);
}

void SkipGramMpiOpenmp::initNegAndUniTable(Dictionary * p2Dict) {
    double z = 0.0;
    double c = 0.0;
    // 遍历哈希链表词典，计算所有的词频加和
    HASHUNITID * htmp = NULL;
    for (size_t i = 0 ; i < TSIZE; i++) {
        if (p2Dict->vocabHash[i] != NULL) {
            htmp = p2Dict->vocabHash[i];
            while (htmp != NULL) {
                if (htmp->id != -1) {
                    z += pow(htmp->Count, 0.5);
                }
            }
        }
    }
    // 按照公式，向负采样表里填入相应个数的id
    for (size_t i = 0 ; i < TSIZE; i++) {
        if (p2Dict->vocabHash[i] != NULL) {
            htmp = p2Dict->vocabHash[i];
            while (htmp != NULL) {
                if (htmp->id != -1) {
                    c = pow(htmp->Count, 0.5);
                }
                // 填入对应个数的id
                for (size_t j = 0; j < c * 10000000 / z; j++) {
                    negatives_.push_back((long long)htmp->id);
                }
            }
        }
    }
    uniform_ = std::uniform_int_distribution<size_t>(0,negatives_.size() - 1);
}

void SkipGramMpiOpenmp::lossEachWin(Dictionary *p2Dict,
                                    Args * p2Args,
                                    int rank,
                                    std::vector<long long> outIds,
                                    GradManager &gradManager) {
    // 将梯度清零
    gradManager.inputGrad.zero();
    double lossTemp=0.0;
    long long negId;
    // 遍历该词窗中的所有正样例
    for (int i = 0; i < outIds.size(); i++) {
        // 计算正样本的loss,积累了input向量的梯度，更新了output向量的梯度
        lossTemp += binaryLogistic(outIds[i], gradManager, true, p2Args);
        // 进行负样本采样，计算负样本的loss，积累input向量的梯度，更新output向量的梯度
        for (int j = 1; j < p2Args->neg; j++) {
            negId = getNegative(outIds[i],gradManager.rng);
            lossTemp += binaryLogistic(negId, gradManager, false, p2Args);
        }
        // 更新inputVec梯度
        SkipGramMpiOpenmp::gradUpdate(gradManager, p2Args);
    }
    lossSG = 0.9 * lossSG + 0.1 * (lossTemp / outIds.size());
}

double SkipGramMpiOpenmp::binaryLogistic(long long outId,
                                         GradManager &gradManager,
                                         bool labelIsPositive,
                                         Args *p2Args) {
    double score = sigmoid(p2Output->dotRow(gradManager.inputVec,outId,1.0));
    double alpha = p2Args->lr * (double(labelIsPositive) - score);
    gradManager.inputGrad.addRow(*p2Output, outId, alpha);
    p2Output->addVectorToRow(gradManager.inputVec, outId, alpha);
    if (labelIsPositive) {
        return -log(score);
    } else {
        return -log(1.0 - score);
    }
}

void SkipGramMpiOpenmp::gradUpdate(GradManager &gradManager, Args * p2Args) {
    p2Input->addVectorToRow(gradManager.inputGrad,gradManager.inId, 1.0);
}

bool SkipGramMpiOpenmp::IfOneEpoch(FILE *p2File, int threadId, int threadNum){
    // todo:这里写了一个大bug。ftell不能返回有意义的文件位置就很坑。
    //首先判断是否为最后一个线程
    if (threadId == threadNum - 1) {
        // 判断条件为是否达到了EOF
        if (feof(p2File)) {
            // 说明一个epoch已经完成，重置到该线程对应启示位置
            return false;
        }
    } else {
        // 判断条件为是否到达了下一个线程的起始位置
        long begRegionNext = (threadId + 1) * size(p2File) / threadNum;
        long positionNow = ftell(p2File);
        if (positionNow > begRegionNext) {
            return false;
        }
    }
    return true;
}

void SkipGramMpiOpenmp::accumuOutput(Dictionary *p2Dict, Args *p2Args, int variId) {
    p2Communicate->addMatrix(*p2Output, 1.0);
    p2Communicate->addFlatMatrix(*p2Dual,1.0 / p2Args->rhoOut, variId);
}

void SkipGramMpiOpenmp::saveSubProSolution(int Id) {
    p2SubProSolution->saveMat2Row(Id,*p2Output);
}

void SkipGramMpiOpenmp::restoreOutput(Dictionary * p2Dict, Args * p2Args) {
    p2Output->zero();
    p2Output->addMatrix(*p2Globe, 1.0);
}

bool SkipGramMpiOpenmp::IfKeepTrain() {
    return true;
}

void SkipGramMpiOpenmp::recordLoss(FILE * p2Record) {
    fprintf(p2Record,"%f/n", lossSG);
}

void SkipGramMpiOpenmp::saveVec(FILE *p2VecFile) {
    int64_t index = 0;
    for (int64_t i = 0; i < p2Globe->rows(); i++) {
        for (int64_t j = 0; j < p2Globe->cols(); j++) {
            index = p2Globe->cols() * i + j;
            fprintf(p2VecFile, "%f ", p2Globe->data()[index]);
        }
        fprintf(p2VecFile, "\n");
    }
}