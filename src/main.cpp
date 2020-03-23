//
// Created by mhyao on 20-1-9.
//
#pragma once
#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>

#include "args.h"
#include "dictionary.h"
#include "matrix.h"
#include "vector.h"
#include "LossFactory.h"


int CompareFile(const void*a, const void*b) {
    long long c;
    c = ((GSIZE *)a)->TotalSize - ((GSIZE *)b)->TotalSize;
    return c < 0 ? 1 : -1;
}

int FindMinIndex(std::vector<GSIZE> &Groups, int ArraySize) {
    if (ArraySize == 1) {
        return 0;
    }
    long long Min=Groups[0].TotalSize;
    int MinIndex = 0;
    for (int i=1; i < ArraySize; i++) {
        if (Groups[i].TotalSize < Min) {
            Min = Groups[i].TotalSize;
            MinIndex = i;
        }
    }
    return MinIndex;
}

void SplitGroups(std::vector<GSIZE> &Groups, long long FileNum, int GroupNum) {
    // 初始化文件数组大小
    auto FileSizeArray = (FSIZE *) malloc(sizeof(FSIZE) * FileNum);
    FILE * ftmp = NULL;
    for (long long i=0; i < FileNum; i++) {
        // 逐个打开文件
        ftmp = fopen(std::to_string(i).c_str(),"r");
        // 判断是否打开成功
        if (ftmp == NULL) {
            throw "Failed to open file";
        }
        // 读取文件大小并存入相应的数组中
        fseek(ftmp,0L,SEEK_END);
        FileSizeArray[i].FileSize = ftell(ftmp);
        fclose(ftmp);
        // 填入对应文件名
        FileSizeArray[i].FileName = i;
    }
    // 应用qsort对上述数组排序
    qsort(FileSizeArray, FileNum, sizeof(FSIZE),CompareFile);
    // 设置组别名称，并贪心分配文件:总是把当前文件给到总文件大小最小的那个组
    // 循环初始化各个文件组对象
    for (int i=0; i < GroupNum; i++) {
        GSIZE gtmp;
        gtmp.TotalSize = 0;
        gtmp.FileNum = 0;
        gtmp.FileNames.clear();
        Groups.push_back(gtmp);
    }
    int MinIndex;
    for (long long i=0; i < FileNum; i++) {
        // 找出当前组别中文件大小最小的那个组
        MinIndex = FindMinIndex(Groups,GroupNum);
        // 将对应文件归入该组，并更新文件组大小
        Groups[MinIndex].TotalSize += FileSizeArray[i].FileSize;
        Groups[MinIndex].FileNames.push_back(FileSizeArray[i].FileName);
        Groups[MinIndex].FileNum += 1;
    }
    // 释放申请的空间
    free(FileSizeArray);
}

int main(int argc, char**argv) {
    // 初始化各个进程
    int rank;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // 各个进程读取用户传入的参数
    // vector的一种初始化方式：给出数据的首地址(argv)，以及结尾的地址(argv+argc)。
    // argv + argc 进行了默认类型转换，是加了argc个string大小的地址。
    std::vector<std::string> args(argv, argv + argc);
    auto arguments = Args();
    bool success = arguments.parseArgs(argc,args);
    // 如果arguments返回了错误false，那就终止程序
    if (!success) {
        return 1;
    }
    // 各个进程初始化LossFactory
    LossFactory lossFactory = LossFactory();
    lossFactory.setHardWare(arguments.hardware);
    success = lossFactory.checkLibary();
    if (!success) {
        return 1;
    }
    lossFactory.setLossName(arguments.loss);
    success = lossFactory.checkSupport();
    if (!success) {
        return 1;
    }
    // 各个进程建立词典对象,建立词汇表
    auto dict = Dictionary();
    dict.setCorpusPath(arguments.input);
    dict.setMaxVocab(arguments.maxVocab);
    dict.setMinCount(arguments.minCount);
    dict.setIfSaveVocab(arguments.ifSaveVocab);
    dict.buildVocab();
    if (rank == 0 && arguments.ifSplitCorpus == 1) { // 只需要一个进程分割文件间就可以了
        dict.splitCorpus(&arguments);
    }
    // 统计语料分组情况
    // 统计所有语料文件的大小,按照大小合理分组语料文件,使得各组训练语料大小大致相同.
    // 并读取进程数量,进程数量即为分组数量
    int GroupNum;
    long long FileNum=dict.getRealVocabSize();
    MPI_Comm_size(MPI_COMM_WORLD, &GroupNum);
    // 定义分组情况的数组, 而后分别存储,一个进程领取一个元素
    std::vector<GSIZE> Groups;
    // 开始统计分组情况
    SplitGroups(Groups,FileNum,GroupNum);
    // 将分组情况塞到字典中去
    dict.setGroups(Groups);
    // myLoss开始初始化和训练
    Loss* p2MyLoss = lossFactory.createLoss(); // creat a loss object on the stack
    p2MyLoss->initVariables(&dict, &arguments, rank);
    p2MyLoss->train(&dict, &arguments, rank);
    MPI_Finalize();
    return 0;
}