//
// Created by mhyao on 2020/4/14.
//

#include"WriterThread.h"
#include "utility.h"
#include <atomic>

WriterThread::WriterThread():_FTWS(NULL),
                             _threadId(0),
                             _threadNum(0),
                             _p2ReadFile(NULL),
                             _p2Args(NULL),
                             _p2Dict(NULL),
                             _Line(0){}

void WriterThread::setThreadId(const int& threadId) {
    _threadId = threadId;
}

void WriterThread::setThreadNum(const int& threadNum) {
    _threadNum = threadNum;
}

void WriterThread::setP2ReadFile(FILE* p2ReadFile) {
    _p2ReadFile = p2ReadFile;
}

FILE * WriterThread::getP2ReadFile() {
    return _p2ReadFile;
}

void WriterThread::setp2FTWS(FileToWriteSync *p2FTWS) {
    _FTWS = p2FTWS;
}

void WriterThread::setp2Dict(Dictionary *p2Dict) {
    _p2Dict = p2Dict;
}

void WriterThread::setp2Args(Args *p2Args) {
    _p2Args = p2Args;
}

void WriterThread::InitReadBegPos(const int& threadNum) {
    std::fseek(_p2ReadFile, _threadId * size(_p2ReadFile) / threadNum, SEEK_SET);
}

void WriterThread::write(std::atomic<long long> &threadProcessedToken, std::vector<FileToWriteSync*> &arrayFTWS) {
    // line存储原始语料中的一行中所有词汇的序号
    std::vector<long long> line;
    // 记录已经处理了多少词窗了
    long long wordWinCount = 0;
    std::string lineTmp;
    while (true) {
        // 如果到了该thread阅读区的结尾，结束
        if (!IfOneEpoch(_p2ReadFile,_threadId,_threadNum)) {
            break;
        }
        // 读取一整行，并转化为序号存入vector容器中
        _p2Dict->getLine(_p2ReadFile, line);
        wordWinCount += line.size();
        threadProcessedToken += line.size();
        // 从同至尾遍历每个序号，建立对应词窗
        for (int i = 0; i < line.size(); i++) {
            // 遇到放弃的词就跳过
            if (line[i] == -1) {
                continue;
            }
            setp2FTWS(arrayFTWS[line[i]]);
            lineTmp.clear();
            for (int j = -_p2Args->ws; j <= _p2Args->ws; j++) {
                // 跳过与自己共现
                if (j == 0) {
                    continue;
                }
                // 检测i+j指向的词是否还在line的范围内
                if (i+j >= 0 && i+j < line.size()) {
                    if (line[i+j] == -1) {
                        continue;
                    }
                    lineTmp += std::to_string(line[i+j]) + "\t";
                }
            }
            // 当前中心词的一个词窗处理完毕，在相应文件中换行
            lineTmp += "\n";
            _FTWS->writeData(lineTmp);
        }
        fprintf(stderr, "\rdone/total:%lld/%lld.", threadProcessedToken.load(), _p2Dict->getTotalTokens());
    }
    fclose(_p2ReadFile);
    fprintf(stderr, "\r\nCorpus split done.\n");
}