//
// Created by mhyao on 2020/1/12.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

typedef struct FileSizeUnit {
    long long FileSize;
    long long FileName;
} FSIZE;

typedef struct FileGroup {
    long long TotalSize;
    std::vector<long long> FileNames;
    long long FileNum;
} GSIZE;

int FineMinIndex(std::vector<GSIZE> Groups, int ArraySize) {
    if (ArraySize == 1) {
        return 0;
    }
    long long Min=Groups[0].TotalSize;
    int MinIndex = 0;
    for (int i=1; i < ArraySize; i++) {
        if (Groups[i].TotalSize < Min) {
            Min = Groups[i].TotalSize;
            MinIndex = i;
        }
    }
    return MinIndex;
}

int CompareFile(const void*a, const void*b) {
    long long c;
    c = ((GSIZE *)a)->TotalSize - ((GSIZE *)b)->TotalSize;
    return c < 0 ? 1 : -1;
}

void printHelp() {
    std::cerr << "\nThe following arguments are mandatory:\n"
              << "  -FileNum            Total file number\n"
              << "  -GroupsNum          Total group number\n";
}

int main (int argc, char** argv) {
    // 读取参数
    long long FileNum;
    int GroupNum;
    std::vector<std::string> args(argv, argv + argc);
    for (int ai = 1; ai < args.size(); ai += 2) {
        if (args[ai][0] != '-') {
            std::cout << "Provided argument without a dash! Usage:" << std::endl;
            printHelp();
            exit(EXIT_FAILURE);
        }
        try {
            if (args[ai] == "-h") {
                std::cerr << "Here is the help! Usage:" << std::endl;
                printHelp();
                exit(EXIT_FAILURE);
            } else if (args[ai] == "-FileNum") {
                FileNum = std::stol(args.at(ai + 1));
            } else if (args[ai] == "-GroupsNum") {
                GroupNum = std::stoi(args.at(ai + 1));
            }
        } catch (std::out_of_range) {
            std::cerr << args[ai] << " is missing an argument" << std::endl;
            printHelp();
            exit(EXIT_FAILURE);
        }
    }
    // 初始化文件数组大小
    auto FileSizeArray = (FSIZE *) malloc(sizeof(FSIZE) * FileNum);
    FILE * ftmp = NULL;
    for (long long i=0; i < FileNum; i++) {
        // 逐个打开文件
        ftmp = fopen(std::to_string(i).c_str(),"r");
        // 判断是否打开成功
        if (ftmp == NULL) {
            throw "Failed to open file";
        }
        // 读取文件大小并存入相应的数组中
        fseek(ftmp,0L,SEEK_END);
        FileSizeArray[i].FileSize = ftell(ftmp);
        fclose(ftmp);
        // 填入对应文件名
        FileSizeArray[i].FileName = i;
    }
    // 应用qsort对上述数组排序
     qsort(FileSizeArray, FileNum, sizeof(FSIZE),CompareFile);
    // 设置组别名称，并贪心分配文件:总是把当前文件给到总文件大小最小的那个组
    std::vector<GSIZE> Groups;
    // 循环初始化各个文件组对象
    for (int i=0; i < GroupNum; i++) {
        GSIZE gtmp;
        gtmp.TotalSize = 0;
        gtmp.FileNum = 0;
        gtmp.FileNames.clear();
        Groups.push_back(gtmp);
    }
    int MinIndex;
    for (long long i=0; i < FileNum; i++) {
        // 找出当前组别中文件大小最小的那个组
        MinIndex = FineMinIndex(Groups,GroupNum);
        // 将对应文件归入该组，并更新文件组大小
        Groups[MinIndex].TotalSize += FileSizeArray[i].FileSize;
        Groups[MinIndex].FileNames.push_back(FileSizeArray[i].FileName);
        Groups[MinIndex].FileNum += 1;
    }
    // 输出分组情况
    for (int i=0; i < GroupNum; i++) {
        fprintf(stderr,"\nThe %dth Group's size is %lld M. It has %lld files. Including:\n\n\t",i,Groups[i].TotalSize / 1024,Groups[i].FileNum);
        for (long long j=0; j < Groups[i].FileNames.size(); j++) {
            fprintf(stderr, "%lld ", Groups[i].FileNames[j]);
        }
        fprintf(stderr, "\n");
    }
    // 释放申请的空间
    free(FileSizeArray);
    return 0;
}