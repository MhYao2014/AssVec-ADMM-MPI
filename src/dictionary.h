//
// Created by mhyao on 20-1-10.
//
#pragma once
#define TSIZE 1048576
#define SEED  1159241
#define MaxWordLen 1000
#define HASHFN  HashValue

#include <iostream>
#include <stdlib.h>
#include <memory>
#include "args.h"

typedef struct VocabHashWithId {
    char *Word;
    long long Count;
    long long id;
    struct VocabHashWithId *next;
} HASHUNITID;

typedef struct VocabUnit {
    char *Word;
    long long Count;
} ARRAYUNIT;

class Dictionary {
protected:
    Args args_;
    long long RealVocabSize;
    unsigned int HashValue(char *word, int tsize, unsigned int seed);
    void HashMapWord(char *Word, HASHUNITID **VocabHash);
    int GetWord(FILE *CorpusFile, char *Word);
    long long HashToArray(HASHUNITID **VocabHash, ARRAYUNIT *VocabArray, long long VocabSize);
    void CutVocab(ARRAYUNIT *VocabArray, long long VocabSize);
    void FillIdToVocabHash(ARRAYUNIT *VocabArray, HASHUNITID **VocabHash);
    void GetLine(FILE *CorpusSplit, std::vector<long long> &line);
//    int GetFileSize();
public:
    HASHUNITID ** VocabHash;
    explicit Dictionary(Args args);
    long long HashSearch(char *Word, HASHUNITID **VocabHash);
    void BuildVocab(FILE *CorpusFile, HASHUNITID **VocabHash);
    HASHUNITID ** Init(int Tsize);
    long long GetWordId(char *Word);
    void SplitCorpus();
};

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