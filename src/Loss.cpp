//
// Created by mhyao on 2020/3/15.
//

#include "Loss.h"
#include <fstream>
Loss::Loss() {
    t_sigmoid_.reserve(512 + 1);
    for (int i = 0; i < 512 + 1; i++) {
        double x = double(i * 2 * 8) / 512 - 8;
        t_sigmoid_.push_back(1.0 / (1.0 + std::exp(-x)));
    }

    t_log_.reserve(512 + 1);
    for (int i = 0; i < 512 + 1; i++) {
        double x = (double(i) + 1e-5) / 512;
        t_log_.push_back(std::log(x));
    }
}

double Loss::sigmoid(double x) {
    if (x < -8) {
        return 0.0;
    } else if (x > 8) {
        return 1.0;
    } else {
        int i =
                int64_t((x + 8) * 512 / 8 / 2);
        return t_sigmoid_[i];
    }
}

double Loss::log(double x) {
    if (x > 1.0) {
        return 0.0;
    }
    int i = int64_t(x * 512);
    return t_log_[i];
}

long long Loss::getNegative(long long id, std::minstd_rand &rng) {
    long long negative;
    do {
        negative = negatives_[uniform_(rng)];
    } while (id == negative);
    return negative;
}

long Loss::size(FILE * p2File) {
    std::fseek(p2File, 0, SEEK_END);
    long size = ftell(p2File);
    return size;
}

void Loss::seek(std::ifstream& ifs, int64_t pos) {
    ifs.clear();
    ifs.seekg(std::streampos(pos));
}