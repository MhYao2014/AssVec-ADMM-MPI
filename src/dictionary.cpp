//
// Created by mhyao on 20-1-10.
//
#pragma once
#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

int scmp(char *s1, char *s2) {
    // 以s1字符串为标准，看s2是不是等于s1
    // 两个字符串不相同时，返回1；
    // 相同时，返回0。
    while (*s1 != '\0' && *s1 == *s2) {s1++;s2++;}
    return *s1 - *s2;
}

int CompareVocab(const void *a, const void *b) {
    long long c;
    if ( (c = ((ARRAYUNIT *)b)->Count - ((ARRAYUNIT *)a)->Count) != 0 ) {
        return c > 0 ? 1 : -1;
    } else {
        return 0;
    }
}

int CompareVocabTie(const void *a, const void *b) {
    long long c;
    if ( (c = ((ARRAYUNIT *) b)->Count - ((ARRAYUNIT *) a)->Count) != 0) return ( c > 0 ? 1 : -1 );
    else return (scmp(((ARRAYUNIT *) a)->Word,((ARRAYUNIT *) b)->Word));
}

HASHUNITID ** Dictionary::Init(int Tsize){
    HASHUNITID **ht = (HASHUNITID **) malloc(sizeof(HASHUNITID *) * Tsize);
    for (int i=0;i<Tsize;i++) {
        ht[i] = (HASHUNITID *) NULL;
    }
    return ht;
}

unsigned int Dictionary::HashValue(char *word, int tsize, unsigned int seed) {
    char c;
    unsigned int h;
    h = seed;
    for ( ; (c = *word) != '\0'; word++) h ^= ((h << 5) + c + (h >> 2));
    return (unsigned int)((h & 0x7fffffff) % tsize);
}

void Dictionary::HashMapWord(char *Word, HASHUNITID **VocabHash) {
    HASHUNITID *hpre= NULL, *hnow= NULL;
    // 首先计算Word字符串的哈希值
    unsigned int hval = HASHFN(Word, TSIZE, SEED);
    // 然后检查是否哈希冲突(当前是否为空，是否和Word内容一致)，
    // 若冲突则用链表解决:指向链表的下一个单元。
    for (   hpre = NULL,hnow = VocabHash[hval];
            hnow != NULL && scmp(hnow->Word, Word)!=0;
            hpre = hnow, hnow = hnow->next );
    // 此时hnow要么为空，要么和Word内容相同，
    // 和Word内容相同时，那就让hnow指向的结构体中的Count+1。
    // 当hnow为空的时候就新开一个小内存，并把这块小内存的地址给hnow,
    // 最后把Word的内容给这块新的内存，Count+1,next设为NULL(让这块新内存变为链表的最后一个节点)。
    if (hnow == NULL) {
        // hnow为空表明该指针没有指向任何内存，所以需要先开辟内存
        // 然后把hnow指向这个内存。
        hnow = (HASHUNITID *) malloc(sizeof(HASHUNITID));
        hnow->Word = (char *) malloc(strlen(Word) + 1);
        strcpy(hnow->Word, Word);
        hnow->Count = 1;
        hnow->id = -1;
        hnow->next = NULL;
        if (hpre == NULL) {
            // hnow指向的那块内存是VocabHash[hval]处的第一个节点
            VocabHash[hval] = hnow;
        } else {
            // 将hnow接到hpre后面
            hpre->next = hnow;
        }
    } else {
        // hnow不为空，就说明遇到同一个词了
        // 将该词Count+1，同时将hnow指向的内存挂在链表的第一个节点
        hnow->Count++;
        // 先判断hnow所指向的内存在链表中是不是已经处在第一个节点了？
        // 如果是的话(表现为hpre指向NULL)，那就不用再移动了;
        // 如果不是的话(表现为hpre不指向NULL)，那hnow就是处在链表中间位置。
        if (hpre != NULL) {
            hpre->next = hnow->next;
            hnow->next = VocabHash[hval];
            VocabHash[hval] = hnow;
        }
    }
    hnow = NULL;
    hpre = NULL;
}

int Dictionary::GetWord(FILE *CorpusFile, char *Word) {
    int i = 0,ch;
    for (;;) {
        ch = fgetc(CorpusFile);
        if (ch == '\r') continue;
        if (i == 0 && ((ch == '\n') || (ch ==EOF) || (ch == '\0') || (ch == '\v') || (ch =='\f'))) {
            Word[i] = 0;
            return 1;
        }
        if (i == 0 && ((ch == ' ') || (ch == '\t'))) {
            continue;
        }
        if (i != 0 && ((ch == EOF) || (ch == ' ') || (ch == '\t') || (ch == '\n') || (ch == '\0') || (ch == '\v') || (ch =='\f'))) {
            if (ch == '\n') {
                ungetc(ch, CorpusFile);
            }
            break;
        }
        if (i < MaxWordLen) {
            // 处理标点符号
            if ((ispunct(ch) || ch == '\'') && i!=0){
                ungetc(ch, CorpusFile);
                Word[i] = 0;
                return 0;
            }
            if ((ispunct(ch)) && i == 0) {
                Word[i++] = ch;
                Word[i] = 0;
                return 0;
            }
            // 大写改成小写
            if (ch > 64 && ch < 91) {
                ch += 32;
            }
            Word[i++] = ch;
        }
    }
    Word[i] = 0;
    // avoid truncation destroying a multibyte UTF-8 char except if only thing on line (so the i > x tests won't overwrite word[0])
    // see https://en.wikipedia.org/wiki/UTF-8#Description
    if (i == MaxWordLen - 1 && (Word[i-1] & 0x80) == 0x80) {
        if ((Word[i-1] & 0xC0) == 0xC0) {
            Word[i-1] = '\0';
        } else if (i > 2 && (Word[i-2] & 0xE0) == 0xE0) {
            Word[i-2] = '\0';
        } else if (i > 3 && (Word[i-3] & 0xF8) == 0xF0) {
            Word[i-3] = '\0';
        }
    }
    return 0;
}

long long Dictionary::HashToArray(HASHUNITID **VocabHash, ARRAYUNIT *VocabArray, long long VocabSize) {
    // 变量定义区，第一次阅读可以跳过，
    // 后面用到再反过来看
    HASHUNITID *htmp= NULL;
    long long ArrayCounter=0;
    // 开始遍历VocabHash的每一行以及每一行中的所有链表
    for (long long HashRow=0;HashRow < TSIZE;HashRow++) {
        htmp = VocabHash[HashRow];
        // 只要htmp不为空，就需要把这一行的所有链表并入数组中
        // 因为这里只是字符串指针的赋值，不需要再开辟内存并拷贝，所以很快
        while (htmp != NULL) {
            VocabArray[ArrayCounter].Word = htmp->Word;
            VocabArray[ArrayCounter].Count = htmp->Count;
            ArrayCounter++;
            // 如果空间不够了，还得再开辟新的内存
            if (ArrayCounter >= VocabSize) {
                VocabSize += 2500;
                VocabArray = (ARRAYUNIT *) realloc(VocabArray, sizeof(ARRAYUNIT) * VocabSize);
            }
            htmp = htmp->next;
        }
    }
    // 这里似乎是不需要再赋一次空指针，
    // 因为当初构造链表的时候保证了链表末尾一定是空指针。
    // Anyway，再写一次也无妨。
    htmp = NULL;
    return ArrayCounter;
}

void Dictionary::CutVocab(ARRAYUNIT *VocabArray, long long VocabSize) {
    // 先砍掉超出最大词汇表长度的单词
    //fprintf(stderr, "\nMaxVocab: %lld; arrarlen: %lld\n", MaxVocab, VocabSize);
    if (args_.maxVocab > 0 && args_.maxVocab < VocabSize)
        qsort(VocabArray, VocabSize, sizeof(ARRAYUNIT), CompareVocab);
    else args_.maxVocab = VocabSize;
    // CompareVocabTie 按照单词首字母的大小裁定两个词频一样的单词谁先谁后
    qsort(VocabArray, args_.maxVocab, sizeof(ARRAYUNIT), CompareVocabTie);
    FILE* VocabFile = fopen("vocab.txt", "w");
    // 从临界词频（刚刚比MinCount大的词频）处截断词表
    for (long long FinalVocabSize=0; FinalVocabSize < args_.maxVocab; FinalVocabSize++) {
        if (VocabArray[FinalVocabSize].Count < args_.minCount) {
            // 将词频不够的单词对应的词字符串指针设为空，count清零。
            VocabArray[FinalVocabSize].Word = NULL;
            VocabArray[FinalVocabSize].Count = 0;
        } else if (args_.IfSaveVocab) {
            fprintf(VocabFile,"%s %lld\n", VocabArray[FinalVocabSize].Word, VocabArray[FinalVocabSize].Count);
        }
    }
    fclose(VocabFile);
    // 将超过最大词汇表长度部分的词串指针设为空，count清零。
    if (args_.maxVocab < VocabSize) {
        for (long long FinalVocabSize=args_.maxVocab; FinalVocabSize < VocabSize; FinalVocabSize++) {
            VocabArray[FinalVocabSize].Word = NULL;
            VocabArray[FinalVocabSize].Count = 0;
        }
    }
}

void Dictionary::FillIdToVocabHash(ARRAYUNIT *VocabArray, HASHUNITID **VocabHash) {
    // 遍历VocabArray，查找其中的每个单词在VocabHash中的位置
    // 将词汇按词频从大到小的顺序编号并存入哈希表中，从0开始编号。
    // 计算当前单词的hash value，这里不采用传入词汇表长度，
    // 而是另外重新计数是为了让这个函数用起来更加self-contained。更加方便。
    long long VocabSize = 0;
    unsigned int hval;
    HASHUNITID *hnow= NULL;
    while (VocabArray[VocabSize].Word != NULL){
        VocabSize += 1;
    }
    for (long long i=0; i < VocabSize; i++) {
        hval = Dictionary::HashValue(VocabArray[i].Word, TSIZE, SEED);
        hnow = VocabHash[hval];
        while (hnow != NULL) {
            if (scmp(hnow->Word,VocabArray[i].Word) == 0) {
                hnow->id = i;
                break;
            } else {
                hnow = hnow->next;
            }
        }
    }
    hnow = NULL;
}

long long Dictionary::HashSearch(char *Word, HASHUNITID **VocabHash) {
    // 如果找不到就返回-1
    // 找得到就返回单词的id值
    long long id = -1;
    unsigned int hval = Dictionary::HashValue(Word,TSIZE,SEED);
    HASHUNITID *htmp = NULL;
    htmp = VocabHash[hval];
    while (htmp != NULL) {
        if (htmp->Word != NULL && scmp(htmp->Word,Word) == 0) {
            id = htmp->id;
            break;
        } else {
            htmp = htmp->next;
        }
    }
    return id;
}

void Dictionary::BuildVocab(FILE *CorpusFile, HASHUNITID **VocabHash) {
    // 变量定义区，第一次阅读可以跳过，
    // 后面用到再反过来看
    int IfNotGet=1;
    char Word[MaxWordLen];
    long long TokenCounter=0;
    long long VocabSize = 1717500;
    ARRAYUNIT *VocabArray = (ARRAYUNIT *)malloc(sizeof(ARRAYUNIT) * VocabSize);
    // 检查是否能够打开语料
    fprintf(stderr, "Building Vocabulary.");
    // 逐个读入CorpusFile中的每个字符，
    // 并将它们初步压入一个哈希表中，
    // 哈希冲突则将单词使用链表挂在后面。
    while (!feof(CorpusFile)) {
        IfNotGet = Dictionary::GetWord(CorpusFile, Word);
        if (IfNotGet) { continue;}
        Dictionary::HashMapWord(Word,VocabHash);
        if (((++TokenCounter) % 100000) == 0) {
            fprintf(stderr, "\rHave read %lld tokens so far.",TokenCounter);
        }
    }
    // 将带链表的哈希表转化为数组array，方便后面排序
    VocabSize = Dictionary::HashToArray(VocabHash,VocabArray,VocabSize);
    fprintf(stderr, "\nCounted %lld unique words.\n", VocabSize);
    // 开始利用最大词表长度以及最小词频限制删减词典,并将词表保存在一个文件中
    Dictionary::CutVocab(VocabArray, VocabSize);
    // 根据VocabArray再给哈希表里每个单词赋予id：
    Dictionary::FillIdToVocabHash(VocabArray,VocabHash);
    // 释放内存，防止出现野指针与内存泄漏。
    // 把被砍掉的词汇（表现为id=-1）对应的字符串释放掉，但是指针本身还得保存，方便后续哈希查找。
    HASHUNITID *htmp= NULL;
    for (int i=0;i<TSIZE;i++) {
        if (VocabHash[i] != NULL) {
            htmp = VocabHash[i];
            while (htmp != NULL) {
                if (htmp->id == -1) {
                    free(htmp->Word);
                    htmp->Word = NULL;
                    htmp->Count = 0;
                }
                htmp = htmp->next;
            }
        }
    }
    htmp = NULL;
    free(VocabArray);
}

Dictionary::Dictionary(Args args) :args_(args) {
    VocabHash = Dictionary::Init(TSIZE);
    FILE* Corpus = fopen(args.input.c_str(), "r");
    Dictionary::BuildVocab(Corpus, VocabHash);
    fclose(Corpus);
}