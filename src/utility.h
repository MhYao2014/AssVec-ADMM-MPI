//
// Created by mhyao on 2020/4/16.
//

#pragma once
#include <iostream>
#include "args.h"
#include "dictionary.h"
#include "FileToWriteSync.h"
#include "WriterThread.h"

bool IfOneEpoch(FILE *p2File, int threadId, int threadNum);

int64_t size(FILE * p2File);

void splitCorpus(Args * p2Args, Dictionary* p2Dict);

void splitCorpusParallel(Args * p2Args, Dictionary* p2Dict);

void splitCorpusThread(std::atomic<long long> &threadProcessedToken, int threadId, int threadNum, Args* p2Args, Dictionary* p2Dict, std::vector<FileToWriteSync*> &arrayFTWS);
