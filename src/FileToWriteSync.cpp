//
// Created by mhyao on 2020/4/14.
//

#include "FileToWriteSync.h"
#include<mutex>

FileToWriteSync::FileToWriteSync(const int &fileNum) : _path(std::to_string(fileNum)) {
    p2file = fopen(_path.c_str(), "a+");
}

FileToWriteSync::FileToWriteSync(const std::string &path) : _path(path) {
    p2file = fopen(_path.c_str(), "a+");
}

void FileToWriteSync::writeData(const std::string& dataToWrite) {
    std::lock_guard<std::mutex> lock(_writerMutex);
    fprintf(p2file, dataToWrite.c_str());
}

void FileToWriteSync::writeLineBreak() {
    fprintf(p2file, "\n");
}

void FileToWriteSync::writeTab() {
    fprintf(p2file, "\t");
}

void FileToWriteSync::closeFile() {
    fclose(p2file);
}