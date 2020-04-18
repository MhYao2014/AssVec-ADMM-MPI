//
// Created by mhyao on 2020/4/14.
//

#ifndef ASSVEC_ADMM_MPI_WRITERTHREAD_H
#define ASSVEC_ADMM_MPI_WRITERTHREAD_H
#pragma once
#include<thread>
#include<string>
#include<vector>
#include<atomic>
#include"FileToWriteSync.h"
#include "dictionary.h"
#include "args.h"

class WriterThread {
public:
    WriterThread();
    void write(std::atomic<long long> &threadProcessedToken, std::vector<FileToWriteSync*> &arrayFTWS);
    void setThreadId(const int& Id);
    void setThreadNum(const int& Num);
    void setP2ReadFile(FILE* p2ReadFile);
    FILE* getP2ReadFile();
    void setp2FTWS(FileToWriteSync* p2FTWS);
    void setp2Dict(Dictionary*p2Dict);
    void setp2Args(Args* p2Args);
    void InitReadBegPos(const int& ThreadNum);
//    void CummulateProcessedDataNum();
private:
    int _threadId;
    int _threadNum;
    Dictionary* _p2Dict;
    Args* _p2Args;
    FILE* _p2ReadFile;
    FileToWriteSync* _FTWS;
    std::vector<int> _Line;
};


#endif //ASSVEC_ADMM_MPI_WRITERTHREAD_H
