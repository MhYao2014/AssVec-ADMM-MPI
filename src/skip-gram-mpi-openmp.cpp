//
// Created by mhyao on 2020/2/10.
//
#pragma once
#include "skip-gram-mpi-openmp.h"
#include "matrix.h"
#include "dictionary.h"
#include "args.h"
#include "utility.h"

#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include <unistd.h>

SkipGramMpiOpenmp::SkipGramMpiOpenmp(): dim(100),
                                        vocabSize(10000),
                                        lossSG(0.0),
                                        p2Input(NULL),
                                        p2InputBackUp(NULL),
                                        p2Dual(NULL),
                                        p2Globe(NULL),
                                        p2Output(NULL),
                                        p2OutputBackUp(NULL),
                                        p2SubProSolution(NULL),
                                        p2Communicate(NULL),
                                        Loss(){}

void SkipGramMpiOpenmp::initVariables(Dictionary *p2Dict, Args *p2Args, int rank) {
    // 设定矩阵行和列的大小
    vocabSize = p2Dict->getRealVocabSize();
    dim = p2Args->dim;
    // 各个进程为自己的模型申请内存空间
    p2Input = std::make_shared<Matrix>(p2Dict->groups[rank].FileNum, dim);
    p2InputBackUp = std::make_shared<Matrix>(p2Dict->groups[rank].FileNum, dim);
    p2Input->uniform(0.01);
    p2InputBackUp->zero();
    p2InputBackUp->addMatrix(*p2Input,1.0);
    p2Dual = std::make_shared<Matrix>(p2Dict->groups[rank].FileNum,dim*vocabSize);
    p2Dual->zero();
//    p2Dual->uniform(0.1);
    p2SubProSolution = std::make_shared<Matrix>(p2Dict->groups[rank].FileNum,dim*vocabSize);
    p2Output = std::make_shared<Matrix>(vocabSize,dim);
    p2OutputBackUp = std::make_shared<Matrix>(vocabSize,dim);
    p2Communicate = std::make_shared<Matrix>(vocabSize, dim);
    p2Globe = std::make_shared<Matrix>(vocabSize, dim);
    if (rank == 0) {
        // 主进程随机初始化output vec参数,
        p2Communicate->uniform(0.01);
        // 开始主进程通信：将主进程的通信数组分配到各个slave进程中的通信数组中去
        MPI_Bcast(p2Communicate->data(), vocabSize*dim, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        // 将通信数组拷贝到缓存数组和工作数组中去。
        p2Globe->zero();
        p2Globe->addMatrix(*p2Communicate, 1);
        p2Output->zero();
        p2Output->addMatrix(*p2Communicate, 1);
        p2OutputBackUp->zero();
        p2OutputBackUp->addMatrix(*p2Output,1.0);
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
        p2OutputBackUp->zero();
        p2OutputBackUp->addMatrix(*p2Output,1.0);
        // 将communicate数组清零，方便后面做局部平均
        p2Communicate->zero();
    }
    // 阻断直到所有slave进程都收到了数据并初始化成功
    MPI_Barrier(MPI_COMM_WORLD);
}

void SkipGramMpiOpenmp::train(Dictionary *p2Dict, Args *p2Args, int rank) {
    // 初始化负采样表。该表位于字典内。
    initNegAndUniTable(p2Dict);
    FILE *p2LossRecord = fopen(("LossRank" + std::to_string(rank) + ".txt").c_str(),"w");
    // 执行ADMM迭代
    for (int AdmmEpo = 0; AdmmEpo < p2Args->epoch; AdmmEpo++) {// 总的ADMM迭代轮数
        fprintf(p2LossRecord, "\nADMM Epoch:%d.\n", AdmmEpo + 1);
        if (rank == 0) {
            fprintf(stderr, "\nADMM Epoch:%d.\n", AdmmEpo + 1);
        }
        // 每个进程遍历自己分组中的各个文件,
        for (int memoId = 0; memoId < p2Dict->groups[rank].FileNum; memoId++) {
            fprintf(p2LossRecord, "\r\tsubProSolved/totalSubPro:%d/%lld:\n", memoId,p2Dict->groups[rank].FileNum);
            if (rank == 0) {
                fprintf(stderr, "\n\r\tsubProSolved/totalSubPro:%d/%lld:\n", memoId,p2Dict->groups[rank].FileNum);
            }
            #pragma omp parallel default(none), \
            shared(memoId, p2Dict, p2Args, rank, stderr), \
            private(p2LossRecord)
            {
                // 请略过下面的私有变量定义区。
                int threadId = omp_get_thread_num(), threadNum= omp_get_num_threads();const int seed = threadId;
                std::default_random_engine dre(seed);std::uniform_int_distribution<int> d(1, 10);
                long long tmpTrainFileId; FILE *p2TrainFile;std::vector<long long> line; int NotReadSuccess = 0;
                long long tempId = 0; GradManager gradient(p2Args->dim, rank);
                //把内存id：memoId转化为字典id：tmpTrainFileName
                tmpTrainFileId = (long long) p2Dict->groups[rank].FileNames[memoId];
                // 取出对应的input向量,将input向量的内存id和词典id存入梯度管理器中
                gradient.inputVec.zero(); p2Input->addRowToVector(gradient.inputVec, memoId, 1.0);
                gradient.inId = memoId;gradient.inDictId = tmpTrainFileId;
                for (int i=0; i < vocabSize; i++) {
                    gradient.outIdCount.push_back(0);
                }
                //将各个线程定位到各自的文件块位置
                p2TrainFile = fopen((p2Args->vocabPath + "/" + std::to_string(tmpTrainFileId)).c_str(), "r");
                std::fseek(p2TrainFile, threadId * size(p2TrainFile) / threadNum, SEEK_SET);
                // 依靠多线程并行(openmp)实现Hogwild!算法。
                for (int subEpo = 0; subEpo < p2Args->subProblemEpoch; subEpo++) {
                    gradient.inputVec.zero();gradient.inputVec.addRow(*p2Input,memoId,1.0);
                    gradient.inputVecBackUp.zero();gradient.inputVecBackUp.addRow(*p2InputBackUp,memoId,1.0);
                    while (IfOneEpoch(p2TrainFile,threadId,threadNum)) {
                        // 读取文件中的一行
                        p2Dict->getNumEachLine(p2TrainFile, tempId, NotReadSuccess, line);
                        if (d(dre) < 8) {
                            continue;
                        }
                        if (!line.empty()) {
                            // 计算loss,计算并更新所有梯度
                            SkipGramMpiOpenmp::lossEachWin(p2Dict, p2Args, rank, line, gradient);
                        }
                        // 调整步长
                        double currentEpoProcess = shrinkLr(gradient,p2TrainFile,threadId,threadNum,subEpo,p2Args->subProblemEpoch,p2Args->lr);
                        // 主进程的主线程输出训练信息
                        if (rank == 0 && threadId == 0) {
                            lossSG = gradient.lossSG / gradient.TotalToken;
                            fprintf(stderr,"\r\t\tsubEpo/total:%d/%d; Process/lr:%lf/%lf; loss:%lf", subEpo+1,p2Args->subProblemEpoch,currentEpoProcess,gradient.lr,lossSG.load());
                        }
                    }
                    p2InputBackUp->zero();p2InputBackUp->addMatrix(*p2Input,1.0);
                    p2OutputBackUp->zero();p2OutputBackUp->addMatrix(*p2Output,1.0);
                    // 一个线程的一个epoch训练完了，重置该线程的文本指针，进入下一轮训练
                    std::fseek(p2TrainFile, threadId * size(p2TrainFile) / threadNum, SEEK_SET);
                }
                fclose(p2TrainFile);// 每求解完一个子问题就关闭其对应文件
            };
            localSumOutput(p2Dict, p2Args, memoId); // 将O_m^k+1与y_m^k累加到通信数组中去
            saveSubProSolution(memoId); // 将O_m^k+1保存下来
            restoreOutput(p2Dict, p2Args); // 用O^k初始化O_m+1^k
        }
        // 平均所有O_m^k+1与y_m^k,清空
        fprintf(stderr,"\n\r\t\t\tStart communication:%d\n", rank);
        MPI_Allreduce(p2Communicate->data(),p2Globe->data(),vocabSize*dim,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);
        fprintf(stderr,"\n\r\t\t\tCommunication done:%d", rank);
        p2Globe->scalerMul(1.0 / (vocabSize)); p2Communicate->zero();
        // 实施梯度上升算法
        for (int memoId = 0; memoId < p2Dict->groups[rank].FileNum; memoId++) {
            p2SubProSolution->addMat2Row(*p2Globe, memoId, -1);
            p2Dual->addRow2Row(*p2SubProSolution,memoId,memoId,p2Args->rhoOut);
        }
        // 主进程保存相关数据,和词向量文件
        if (rank == 0) {
            FILE * p2VecFile = fopen((p2Args->output + std::to_string(AdmmEpo+1)).c_str(),"w");
            saveVec(p2VecFile);
            fclose(p2VecFile);
        }
    }
    // ADMM算法结束,
    fclose(p2LossRecord);
}

void SkipGramMpiOpenmp::lossEachWin(Dictionary *p2Dict,
                                    Args * p2Args,
                                    int rank,
                                    std::vector<long long> outIds,
                                    GradManager &gradient) {
    double lossTemp;
    long long negId;
    // 遍历该词窗中的所有正样例
    for (auto&outId: outIds) {
        // 每个正样本清空一次inputGrad，对应更新一次inputvec的负梯度
        gradient.inputGrad.zero();
        gradient.TotalToken += 1;
        gradient.outIdCount[outId] += 1;
        lossTemp=0.0;
        // 计算正样本的loss,积累了input向量的梯度，更新了output向量的梯度
        lossTemp += binaryLogistic(outId, gradient, true, p2Args);
        // 进行负样本采样，计算负样本的loss，积累input向量的梯度，更新output向量的梯度
        for (int j = 1; j < p2Args->neg; j++) {
            negId = getNegative(outId,gradient.rng);
            gradient.outIdCount[negId] += 1;
            lossTemp += binaryLogistic(negId, gradient, false, p2Args);
        }
        // 更新inputVec梯度
        SkipGramMpiOpenmp::gradUpdate(gradient, p2Args);
        gradient.lossSG += lossTemp;

    }
}

double SkipGramMpiOpenmp::binaryLogistic(long long outId,
                                         GradManager &gradManager,
                                         bool labelIsPositive,
                                         Args *p2Args) {
    // 强迫各个O^m向O靠拢的负梯度，更新outvec的负梯度，积累invec的负梯度
    gradManager.outputVec.zero();gradManager.outputVecBackUp.zero();
    gradManager.outputVec.addRow(*p2Output,outId,1);gradManager.outputVecBackUp.addRow(*p2OutputBackUp,outId,1.0);
    gradManager.outputGrad.zero();
    gradManager.outputGrad.addVector(gradManager.outputVec,-p2Args->rhoOut);
    gradManager.outputGrad.addRow(*p2Globe,outId,p2Args->rhoOut);
    gradManager.outputGrad.addRowTensor(*p2Dual,gradManager.inId,outId,-1);
    // Hogwild!部分的负梯度，更新outvec的负梯度，积累invec的负梯度
    double score = sigmoid(p2Output->dotRow(gradManager.inputVec,outId,1.0));
    double alpha = gradManager.lr * (double(labelIsPositive) - score);
    gradManager.inputGrad.addRow(*p2OutputBackUp, outId, alpha*(1/(double)gradManager.TotalToken));
    p2Output->addVectorToRow(gradManager.inputVecBackUp, outId, alpha*(1/(double)gradManager.outIdCount[outId]));
    p2Output->addVectorToRow(gradManager.outputGrad,outId, gradManager.lr);
    if (labelIsPositive) {
        return -log(score);
    } else {
        return -log(1.0 - score);
    }
}

void SkipGramMpiOpenmp::gradUpdate(GradManager &gradManager, Args * p2Args) {
    p2Input->addVectorToRow(gradManager.inputGrad, gradManager.inId, 1.0);
}

void SkipGramMpiOpenmp::localSumOutput(Dictionary *p2Dict, Args *p2Args, int variId) {
    p2Communicate->addMatrix(*p2Output, 1.0);
    p2Communicate->addFlatMatrix(*p2Dual,1.0 / p2Args->rhoOut, variId);
}

void SkipGramMpiOpenmp::saveSubProSolution(int Id) {
    p2SubProSolution->saveMat2Row(Id,*p2Output);
}

void SkipGramMpiOpenmp::initNegAndUniTable(Dictionary * p2Dict) {
    double z = 0.0;
    double c = 0.0;
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

void SkipGramMpiOpenmp::restoreOutput(Dictionary * p2Dict, Args * p2Args) {
    p2Output->zero();
    p2Output->addMatrix(*p2Globe, 1.0);
}