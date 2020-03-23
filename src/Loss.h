//
// Created by mhyao on 2020/2/9.
//
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <random>
#include "dictionary.h"
#include "args.h"

class Loss {
protected:
    std::vector<double> t_sigmoid_;
    std::vector<double> t_log_;
    std::vector<long long> negatives_;
    std::uniform_int_distribution<size_t> uniform_;
public:
    explicit Loss();
    virtual ~Loss() = default;

    virtual void initVariables(Dictionary *p2Dict, Args *p2Args);

    virtual void initVariables(Dictionary *p2Dict, Args *p2Args, int rank);

    virtual void train(Dictionary *p2Dict, Args *p2Args, int rank);

    double sigmoid(double x);

    double log(double x);

    long long getNegative(long long id, std::minstd_rand & rng);

    int64_t size(FILE * p2File);

    void seek(std::ifstream&, int64_t);
};