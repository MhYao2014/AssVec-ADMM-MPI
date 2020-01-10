//
// Created by mhyao on 20-1-9.
//

//#include <mpi.h>
//#include <iostream>
//int main(int argv, char**args) {
//    int err,rank;
//    err = MPI_Init(&argv, &args);
//    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
//    // 语料处理和分发.
//    if (rank == 0) {
//        // root进程首先统计原始语料的词频,建立词典
//        // 并且给每个词按照词频从大到小赋予编号
//
//        // 接着主节点从原始语料中,分词汇,
//        // 逐词抽取训练语料.具体而言,
//        // 每个词汇的训练语料为所有出现在该词汇词窗
//        // 中的单词的编号.同一个词窗的编号占一行.
//
//        // 统计所有语料文件的大小,
//        // 按照大小合理分组语料文件,
//        // 使得各组训练语料大小大致相同.
//
//        // 根据语料分组实现进程分组
//
//        // 最后将各组语料发往各组进程所在节点,
//        // 并等待通信结束.
//    } else {
//        // 非root进程接受训练语料.
//    }
//
//    // 语料接受完毕,各进程所在节点本地已经有语料
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
//
//    return 0;
//}
#include <stdio.h>
#include "dictionary.h"
int main(int argc, char**argv) {
    if (argc < 2) {
        printf("%s\n", argv[0]);
    }
    // root进程首先统计原始语料的词频,建立词典
    // 并且给每个词按照词频从大到小赋予编号
    Dictionary dict;

    return 0;
}