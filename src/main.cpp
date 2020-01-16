//
// Created by mhyao on 20-1-9.
//

int ReadDocumentsNum() {
    return 0;
}

#include <openmpi/mpi.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "args.h"

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
    int err,rank;
    err = MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<std::string> args(argv, argv + argc);
    Args arguments = Args();
    if (rank == 0) {
        // root进程提供打印服务
        if (args.size() < 2) {
            arguments.printHelp();
            // 停止所有进程
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }
    // 统计所有语料文件的大小,
    // 按照大小合理分组语料文件,
    // 使得各组训练语料大小大致相同.
    // 读取进程数量,进程数量即为分组数量
    int GroupNum;
    long long FileNum=100000;
    MPI_Comm_size(MPI_COMM_WORLD, &GroupNum);
    // 定义分组情况的数组, 而后分别存储,一个进程领取一个元素
    std::vector<GSIZE> Groups;
    // 开始统计分组情况
    SplitGroups(Groups,FileNum,GroupNum);
    // 输出分组情况

    fprintf(stderr,"\nThe %dth Group's size is %lld M. It has %lld files. Including:\n\n\t",rank,Groups[rank].TotalSize / 1020 / 1020,Groups[rank].FileNum);
    for (long long j=0; j < Groups[rank].FileNames.size(); j++) {
        fprintf(stderr, "%lld ", Groups[rank].FileNames[j]);
    }
    fprintf(stderr, "\n");
//    // 开始分情况考虑本地训练问题
//    if (rank == 0) {
//        // root节点需要训练,
//        // 只需要负责和各个进程组的root节点进行计算平均梯度.
//    } else {
//        // 非root节点需要区分是否处理多个训练语料文件
//        int documentNum = ReadDocumentsNum();
//        if (documentNum == 1) {
//            // 处理单个大训练语料的进程组
//
//            // 计算完成后,将参数同主进程进行通信
//        } else if (documentNum > 1) {
//            // 处理多个小的训练语料的进程组
//        }
//    }

    return 0;
}
//#include <stdio.h>
//#include <fstream>
//#include "dictionary.h"
//#include "args.h"
//#include "/usr/local/openmpi/include/mpi.h"
//int main(int argc, char**argv) {
//    // 从命令行读取参数
//    std::vector<std::string> args(argv, argv + argc);
//    Args arguments = Args();
//    if (args.size() < 2) {
//        arguments.printHelp();
//        exit(EXIT_FAILURE);
//    }
//    arguments.parseArgs(args);
//    // root进程首先统计原始语料的词频,建立词典
//    // 并且给每个词按照词频从大到小赋予编号
//    // 首先检查能否打开原始训练语料
//    std::ifstream CorpusCheck(arguments.input);
//    if (!CorpusCheck.is_open()) {
//        throw std::invalid_argument(arguments.input + "cannot be opened for training");
//    }
//    CorpusCheck.close();
//    // 初始化一个词典,并建立词表
//    // 并且给每个词按照词频从大到小赋予编号
//    Dictionary dict(arguments);
//    // 记得释放内存，避免内存泄漏
//
//    // 接着主节点从原始语料中,分词汇,
//    // 逐词抽取训练语料.具体而言,
//    // 每个词汇的训练语料为所有出现在该词汇词窗
//    // 中的单词的编号.同一个词窗的编号占一行.
//    dict.SplitCorpus();
//    HASHUNITID *htmp= NULL,*hpre= NULL;
//    for (int i=0;i<TSIZE;i++) {
//        if (dict.VocabHash[i] != NULL) {
//            htmp = dict.VocabHash[i];
//            while (htmp != NULL) {
//                free(htmp->Word);
//                hpre = htmp;
//                htmp = htmp->next;
//                free(hpre);
//                hpre = NULL;//防止hpre变成野指针，随机指向某个内存区域
//            }
//        }
//    }
//    free(dict.VocabHash);
//    htmp = NULL;
//    hpre = NULL;
//    return 0;
//}
//HASHUNITID *htmp= NULL,*hpre= NULL;
//for (int i=0;i<TSIZE;i++) {
//if (dict.VocabHash[i] != NULL) {
//htmp = dict.VocabHash[i];
//while (htmp != NULL) {
//free(htmp->Word);
//hpre = htmp;
//htmp = htmp->next;
//free(hpre);
//hpre = NULL;//防止hpre变成野指针，随机指向某个内存区域
//}
//}
//}
//free(dict.VocabHash);
//htmp = NULL;
//hpre = NULL;