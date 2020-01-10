//
// Created by mhyao on 20-1-10.
//

#ifndef MULTITHREAD_DICTIONARY_H
#define MULTITHREAD_DICTIONARY_H
#define TSIZE 1048576
#define SEED  1159241
#define MaxWordLen 1000
#define HASHFN  HashValue
#endif //MULTITHREAD_DICTIONARY_H

#include <iostream>
#include <stdlib.h>
#include <memory>

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
public:
    unsigned int HashValue(char *word, int tsize, unsigned int seed);
    void HashMapWord(char *Word, HASHUNITID **VocabHash);
    int GetWord(FILE *CorpusFile, char *Word);
    long long HashToArray(HASHUNITID **VocabHash, ARRAYUNIT *VocabArray, long long VocabSize);
    void CutVocab(ARRAYUNIT *VocabArray, long long MaxVocab, long long MinCount, long long VocabSize, int IfSaveVocab);
    void FillIdToVocabHash(ARRAYUNIT *VocabArray, HASHUNITID **VocabHash);

public:
    HASHUNITID ** VocabHash;
    explicit Dictionary(std::shared_ptr<>);
    long long HashSearch(char *Word, HASHUNITID **VocabHash);
    ARRAYUNIT * BuildVocab(FILE *CorpusFile, HASHUNITID **VocabHash,long long MaxVocab, long long MinCount, int IfSaveVocab);
    HASHUNITID ** Init(int Tsize);
};