//
// Created by mhyao on 2020/2/6.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "args.h"
#include "Loss.h"

class LossFactory {
private:
    std::string _hard_ware;
    std::string _loss_name;
    bool _lib_can_load;

public:
    explicit LossFactory();
    void setHardWare(std::string &hard_ware);
    bool checkLibary();
    void setLossName(std::string &loss_name);
    bool checkSupport();
    Loss *createLoss();
};