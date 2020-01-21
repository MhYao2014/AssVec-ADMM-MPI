//
// Created by mhyao on 20-1-22.
//
#include "vector.h"
#include <cmath>
#include <assert.h>

Vector::Vector(int64_t m): data_(m) {}

void Vector::zero() {
    std::fill(data_.begin(), data_.end(), 0.0);
}

double Vector::norm() {
    double sum = 0;
    for (int64_t i = 0; i < size(); i++) {
        sum += data_[i] * data_[i];
    }
    return std::sqrt(sum);
}

void Vector::scalerMul(double a) {
    for (int64_t i = 0; i < size(); i++) {
        data_[i] *= a;
    }
}

double Vector::dotMul(Vector &vec, double a) {
    assert(vec.size() == size());
    double result = 0;
    for (int64_t i = 0; i < size(); i++) {
        result += data_[i] * vec.data_[i] * a;
    }
    return result;
}

void Vector::addVector(Vector &vec, double a) {
    assert(size() == vec.size());
    for (int64_t i = 0; i < size(); i++) {
        data_[i] += a * vec.data_[i];
    }
}

void Vector::addRow(Matrix &mat, int64_t i, double a) {
    assert(i >= 0);
    assert(i < mat.cols());
    assert(size() == mat.rows());
    mat.addRowToVector(*this, i, a);
}