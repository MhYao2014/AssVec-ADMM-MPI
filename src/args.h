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
    Args();
    std::string input;
    std::string output;
    int IfSaveVocab;
    double lr;
    int lrUpdateRate;
    int dim;
    int ws;
    int epoch;
    int minCount;
    int maxVocab;
    int neg;
    int thread;
    void parseArgs(const std::vector<std::string>& args);
    void printHelp();
};