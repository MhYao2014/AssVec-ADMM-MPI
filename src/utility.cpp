//
// Created by mhyao on 2020/4/16.
//

#include "utility.h"
#include <atomic>
#include <unistd.h>

bool IfOneEpoch(FILE *p2File, int threadId, int threadNum){
    // todo:这里写了一个大bug。ftell不能返回有意义的文件位置就很坑。
    // 判断条件为是否到达了下一个线程的起始位置
    long begRegionNext = (threadId + 1) * size(p2File) / threadNum;
    long positionNow = ftell(p2File);
    if (positionNow >= begRegionNext) {
        return false;
    }
    return true;
}

long size(FILE * p2File) {
    long tmpPos = ftell(p2File);
    std::fseek(p2File, 0, SEEK_END);
    long size = ftell(p2File);
    std::fseek(p2File,tmpPos, SEEK_SET);
    return size;
}

void splitCorpus(Args * p2Args, Dictionary* p2Dict) {
    // 打开原始语料文件
    FILE * CorpusSplit = fopen(p2Args->input.c_str(),"r");
    // 设立一个临时文件指针，用来指向每个遇到的文件
    FILE * ftmp = NULL;
    // line存储原始语料中的一行中所有词汇的序号
    std::vector<long long> line;
    // 记录已经处理了多少词窗了
    long long wordWinCount = 0;

    while (true) {
        // 如果原始语料到了文件末尾就跳出循环
        if (feof(CorpusSplit)) {
            break;
        }
        // 读取一整行，并转化为序号存入vector容器中
        p2Dict->getLine(CorpusSplit, line);
        // 从同至尾遍历每个序号，建立对应词窗
        for (int i = 0; i < line.size(); i++) {
            // 检测当前词汇是否已经创立了对应的文件
            // 如果没有那就创建，否则打开对应文件并写入数据
            // 遇到放弃的词就跳过
            if (line[i] == -1) {
                continue;
            }
            ftmp = fopen((p2Args->vocabPath+"/"+std::to_string(line[i])).c_str(),"a+");
            if (ftmp == NULL) {
                throw "can not open file";
            }
            for (int j = -p2Args->ws; j <= p2Args->ws; j++) {
                // 跳过与自己共现
                if (j == 0) {
                    continue;
                }
                // 检测i+j指向的词是否还在line的范围内
                if (i+j >= 0 && i+j < line.size()) {
                    if (line[i+j] == -1) {
                        continue;
                    }
                    fprintf(ftmp, "%s\t", std::to_string(line[i+j]).c_str());
                }
            }
            // 当前中心词的一个词窗处理完毕，在相应文件中换行
            fprintf(ftmp, "\n");
            fclose(ftmp);
        }
        // 记录处理过的词窗个数
        wordWinCount += line.size();
        if (wordWinCount % 100000 == 0) {
            fprintf(stderr, "\rHave processed %lld word windows", wordWinCount);
        }
    }
}

void splitCorpusThread(std::atomic<long long> &threadProcessedToken, int threadId, int threadNum, Args* p2Args, Dictionary* p2Dict, std::vector<FileToWriteSync *> &arrayFTWS) {
    auto writer = WriterThread();
    writer.setThreadId(threadId);
    writer.setThreadNum(threadNum);
    writer.setp2Args(p2Args);
    writer.setp2Dict(p2Dict);
    FILE * p2ReadFile;
    p2ReadFile = fopen(p2Args->input.c_str(),"r");
    writer.setP2ReadFile(p2ReadFile);
    writer.InitReadBegPos(threadNum);
    writer.write(threadProcessedToken,arrayFTWS);
}

void splitCorpusParallel(Args *p2Args, Dictionary* p2Dict) {
    // 打开并建立了_real_vocab_size个语料文件，等待写入。
    std::vector<FileToWriteSync *> arrayP2FTWS;
    for (int i = 0; i < p2Dict->getRealVocabSize(); i++) {
        auto tmp = new FileToWriteSync(p2Args->vocabPath+"/"+std::to_string(i));
        arrayP2FTWS.push_back(tmp);
    }
    // 打开多线程
    int threadNum = p2Args->corpusSplitThread;
    std::vector<std::thread> threadPool;
    std::atomic<long long> threadProcessedToken(0);
    for (int j = 0; j < threadNum; j++) {
        threadPool.push_back(std::thread(splitCorpusThread,std::ref(threadProcessedToken),j,threadNum,p2Args,p2Dict,std::ref(arrayP2FTWS)));
    }
    // 关闭多线程
    for (auto&t: threadPool) {
        t.join();
    }
    // 关闭所有文件
    int filecount = 0;
    for (auto&f: arrayP2FTWS) {
        filecount += 1;
        fprintf(stderr, "\rClosed file:%d.",filecount);
        f->closeFile();
    }
}