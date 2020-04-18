//
// Created by mhyao on 2020/4/14.
//
#pragma once
#include<string.h>
#include<string>
#include<mutex>
#include<stdlib.h>
#include<stdio.h>
#ifndef ASSVEC_ADMM_MPI_FILETOWRITESYNC_H
#define ASSVEC_ADMM_MPI_FILETOWRITESYNC_H


class FileToWriteSync {
public:
    FileToWriteSync(const std::string& path);
    FileToWriteSync(const int& fileNum);
    void closeFile();
    void writeData(const std::string& dataToWrite);
    void writeLineBreak();
    void writeTab();
private:
    std::string _path;
    std::mutex _writerMutex;
    FILE* p2file = NULL;
};


#endif //ASSVEC_ADMM_MPI_FILETOWRITESYNC_H
