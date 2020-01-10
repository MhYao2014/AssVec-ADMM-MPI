//
// Created by mhyao on 20-1-10.
//
#pragma once
#include "args.h"
#include <stdlib.h>
#include <iostream>

Args::Args() {
    IfSaveVocab = 1;
    lr = 0.05;
    lrUpdateRate = 100;
    dim = 100;
    ws = 5;
    epoch = 5;
    minCount = 5;
    maxVocab = 1000000;
    neg = 5;
    thread = 12;
}

void Args::printHelp() {
    std::cerr << "\nThe following arguments are mandatory:\n"
              << "  -output             output vectors path\n"
              << "  -input              training file path\n"
              << "  -IfSaveVocab        whether save the vocab file\n"
              << "\nThe following arguments for training are optional:\n"
              << "  -lr                 learning rate [" << lr << "]\n"
              << "  -lrUpdateRate       change the rate of updates for the learning rate ["
              << lrUpdateRate << "]\n"
              << "  -dim                size of word vectors [" << dim << "]\n"
              << "  -ws                 size of the context window [" << ws << "]\n"
              << "  -epoch              number of epochs [" << epoch << "]\n"
              << "  -minCount           minimal number of word occurences ["
              << minCount << "]\n"
              << "  -neg                number of negatives sampled [" << neg << "]\n"
              << "  -thread             number of threads [" << thread << "]\n";
}

void Args::parseArgs(const std::vector<std::string> &args) {
    for (int ai = 1; ai < args.size(); ai += 2) {
        if (args[ai][0] != '-') {
            std::cout << "Provided argument without a dash! Usage:" << std::endl;
            Args::printHelp();
            exit(EXIT_FAILURE);
        }
        try {
            if (args[ai] == "-h") {
                std::cerr << "Here is the help! Usage:" << std::endl;
                printHelp();
                exit(EXIT_FAILURE);
            } else if (args[ai] == "-input") {
                input = std::string(args.at(ai + 1));
            } else if (args[ai] == "-output") {
                output = std::string(args.at(ai + 1));
            } else if (args[ai] == "-IfSaveVocab") {
                IfSaveVocab = std::stoi(args.at(ai + 1));
            } else if (args[ai] == "-lr") {
                lr = std::stof(args.at(ai + 1));
            } else if (args[ai] == "-lrUpdateRate") {
                lrUpdateRate = std::stoi(args.at(ai + 1));
            } else if (args[ai] == "-dim") {
                dim = std::stoi(args.at(ai + 1));
            } else if (args[ai] == "-ws") {
                ws = std::stoi(args.at(ai + 1));
            } else if (args[ai] == "-epoch") {
                epoch = std::stoi(args.at(ai + 1));
            } else if (args[ai] == "-minCount") {
                minCount = std::stoi(args.at(ai + 1));
            } else if (args[ai] == "-neg") {
                neg = std::stoi(args.at(ai + 1));
            } else if (args[ai] == "-thread") {
                thread = std::stoi(args.at(ai + 1));
            }
        } catch (std::out_of_range) {
            std::cerr << args[ai] << " is missing an argument" << std::endl;
            printHelp();
            exit(EXIT_FAILURE);
        }
    }
}