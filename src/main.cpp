//
// Created by mhyao on 20-1-9.
//
#include <mpi.h>
//#include <omp.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include "args.h"
#include "dictionary.h"
#include "matrix.h"
#include "vector.h"

typedef struct FileSizeUnit {
    long long FileSize;
    long long FileName;
} FSIZE;

typedef struct FileGroup {
    long long TotalSize;
    std::vector<long long> FileNames;
    long long FileNum;
} GSIZE;

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
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // 各个进程读取模型的超参数
    std::vector<std::string> args(argv, argv + argc);
    Args arguments = Args();
    // root进程提供打印服务
    if (rank == 0) {
        if (args.size() < 2) {
            arguments.printHelp();
            // 停止所有进程
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }
    // 统计所有语料文件的大小,按照大小合理分组语料文件,使得各组训练语料大小大致相同.
    // 并读取进程数量,进程数量即为分组数量
    int GroupNum;
    long long FileNum=1223;
    MPI_Comm_size(MPI_COMM_WORLD, &GroupNum);
    // 定义分组情况的数组, 而后分别存储,一个进程领取一个元素
    std::vector<GSIZE> Groups;
    // 开始统计分组情况
    SplitGroups(Groups,FileNum,GroupNum);
    // 主进程输出分组情况
//    if (rank == 0) {
//        fprintf(stderr,"\nThe %dth Group's size is %lld M. It has %lld files. Including:\n\n\t",rank,Groups[rank].TotalSize / 1020 / 1020,Groups[rank].FileNum);
//        for (long long j=0; j < Groups[rank].FileNames.size(); j++) {
//            fprintf(stderr, "%lld ", Groups[rank].FileNames[j]);
//        }
//        fprintf(stderr, "\n");
//    }
//    // 各进程等待调试
    volatile int i = 0;
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    printf("Process %d PID %d on %s ready for attach\n", rank,getpid(), hostname);
    fflush(stdout);
//    while (0 == i) {
//        sleep(5);
//    }
    // 各个进程初始化自己的模型参数,创建训练对象,随机初始化input vec参数,
    Matrix Input(Groups[rank].FileNum, arguments.dim);
    Input.uniform(0.1);// 随机初始化input vec参数
    Matrix Output(1223, arguments.dim);// 工作数组
    Matrix Communicate(1223, arguments.dim);// 通信数组
    Matrix Catch(1223, arguments.dim);// 缓存数组
    if (rank != 0) {
        // 非主进程，初始化为0,通信数组累加本分组的参数
        Output.zero();
        Catch.zero();
    }
    if (rank == 0) {
        // 主进程随机初始化output vec参数,
        Communicate.uniform(0.1);
        // 开始主进程通信：将主进程的通信数组分配到各个slave进程中的通信数组中去
        MPI_Bcast(Communicate.data(),1223*arguments.dim,MPI_DOUBLE,0,MPI_COMM_WORLD);
        // 将通信数组拷贝到缓存数组和工作数组中去,
        Catch.zero();
        Catch.addMatrix(Communicate,1);
        Output.zero();
        Output.addMatrix(Communicate, 1);
    }
    // slave进程会先结束，从而先开始运行此段代码
    // slave开始等待主进程发送消息,统一采用阻塞通信
    if (rank != 0) {
        MPI_Bcast(Communicate.data(),1223*arguments.dim,MPI_DOUBLE,0,MPI_COMM_WORLD);
        // 将通信数组拷贝到缓存数组和工作数组中去,
        Catch.zero();
        Catch.addMatrix(Communicate,1);
        Output.zero();
        Output.addMatrix(Communicate, 1);
    }
    // 阻断直到所有slave进程都收到了数据并初始化成功
    MPI_Barrier(MPI_COMM_WORLD);
    //执行ADMM迭代
        // 每个进程遍历自己分组中的各个文件,
            // 依靠多线程并行(openmp)迭代固定次数,求解子问题(各个文件),
            // 每求解完一个子问题,就将当前参数累加到通信数组中去
            // 将缓存数组中当前轮数的原始参数复制到工作数组中
            // 进入下一个问题的求解.

        // 所有进程的所有子问题求解完毕,
        // 同主进程同步通信(缓存数组),平均参数

        // 实施梯度上升算法

        // 主进程判断残差是否收敛,或者是否达到最大迭代次数
        // 同时记录训练日志

    // ADMM算法结束,
    // 主进程保存相关数据,和词向量文件
    MPI_Finalize();
    return 0;
}
