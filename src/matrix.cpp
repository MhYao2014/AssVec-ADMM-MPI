//
// Created by mhyao on 20-1-21.
//
#include "matrix.h"
#include "vector.h"
#include <vector>
#include <stdio.h>
#include <random>
#include <assert.h>
#include <stdexcept>

Matrix::Matrix(): row_(0), col_(0) {}

Matrix::Matrix(int64_t row, int64_t col): row_(row), col_(col) ,data_(row * col){}

void Matrix::zero() {
    std::fill(data_.begin(), data_.end(), 0.0);
}

void Matrix::uniform(double a) {
    std::minstd_rand rng(1);
    std::uniform_real_distribution<> uniform(-a, a);
    for (int64_t i=0; i < (row_ * col_); i++) {
        data_[i] = uniform(rng);
    }
}

void Matrix::scalerMul(double a) {
    for (int64_t j = 0; j < data_.size(); j++) {
        data_[j] = a * data_[j];
    }
}

double Matrix::l2NormRow(int64_t i) {
    auto norm = 0.0;
    for (auto j = 0; j < col_; j++) {
        norm += at(i, j) * at(i, j);
    }
    if (std::isnan(norm)) {
        throw std::runtime_error("Encountered NaN.");
    }
    return std::sqrt(norm);
}

double Matrix::dotRow(Vector& vec, int64_t i, double a) {
    assert(i >= 0);
    assert(i < row_);
    assert(vec.size() == col_);
    double d = 0.0;
    for (int64_t j = 0; j < col_; j++) {
        d += at(i,j) * vec[j];
    }
    if (std::isnan(d)) {
        throw std::runtime_error("Encountered NaN.");
    }
    return d;
}

void Matrix::addVectorToRow(Vector &vec, int64_t i, double a) {
    assert(i >= 0);
    assert(i < row_);
    assert(vec.size() == col_);
    for (int64_t j = 0; j < col_; j++) {
        data_[i * col_ + j] += a * vec[j];
    }
}

void Matrix::addRowToVector(Vector &vec, int64_t i, double a) {
    assert(i >= 0);
    assert(i < row_);
    assert(vec.size() == col_);
    for (int64_t j = 0; j < col_; j++) {
        vec[j] += a * at(i, j);
    }
}

void Matrix::addMatrix(Matrix &mat, double a) {
    assert(row_ == mat.row_);
    assert(col_ == mat.col_);
    for (int64_t j = 0; j < mat.data_.size(); j++) {
        data_[j] += a * mat.data_[j];
    }
}