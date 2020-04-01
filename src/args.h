//
// Created by mhyao on 20-1-10.
//
#pragma once

#include <istream>
#include <ostream>
#include <string>
#include <vector>

class Args {
public:
    std::string input;
    std::string output;
    std::string vocabPath;
    std::string hardware;
    std::string loss;
    int ifSaveVocab;
    int ifSplitCorpus;
    int minCount;
    int maxVocab;
    std::string vocabSavePath;
    double lr;
    double rhoOut;
    int lrUpdateRate;
    int dim;
    int ws;
    int epoch;
    int subProblemEpoch;
    int neg;
    std::string vecSavePath;
    explicit Args();
    bool parseArgs(int argc,const std::vector<std::string>& args);
    void printHelp();
};