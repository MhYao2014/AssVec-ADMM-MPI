//
// Created by mhyao on 2020/2/9.
//
#include "LossFactory.h"
#include "skip-gram-mpi-openmp.h"
#include "SkipGramSubproblem.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

LossFactory::LossFactory(): _lib_can_load(false) {}

void LossFactory::setHardWare(std::string & hard_ware) {
    _hard_ware = hard_ware;
}

bool LossFactory::checkLibary() {
    _lib_can_load = true;

    if (_lib_can_load) {
        return true;
    } else {
        return false;
    }
}

void LossFactory::setLossName(std::string & loss_name) {
    _loss_name = loss_name;
}

bool LossFactory::checkSupport() {
    // 判断(loss, hardware)是否已经实现过了
    return true;
}

Loss* LossFactory::createLoss() {
    if (_loss_name == "skip-gram-mpi-openmp") {
        // 在堆上创建具体化一个匿民对象
        // 并用一个对象指针指向这个对象在堆上的地址
        auto* p2myloss = new SkipGramMpiOpenmp;
        return p2myloss;
    } else if (_loss_name == "skip-gram-subPro") {
        auto* p2myloss = new SkipGramSubproblem;
        return p2myloss;
    }
}