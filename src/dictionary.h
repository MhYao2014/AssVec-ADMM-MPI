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
public:
    HASHUNITID ** VocabHash;
    explicit Dictionary(Args args);
    long long HashSearch(char *Word, HASHUNITID **VocabHash);
    void BuildVocab(FILE *CorpusFile, HASHUNITID **VocabHash);
    HASHUNITID ** Init(int Tsize);
    long long GetWordId(char *Word);
    void SplitCorpus();
};