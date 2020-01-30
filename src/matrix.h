//
// Created by mhyao on 20-1-21.
//
#pragma once
#include <vector>
#include <cstdint>
// 因为Matrix和Vector相互引用,这里是前置申明
class Vector;

class Matrix {
protected:
    int64_t row_;
    int64_t col_;
    std::vector<double> data_;

public:
    // 构造函数
    Matrix();
    explicit Matrix(int64_t, int64_t);
    // 析构函数
    virtual ~Matrix() noexcept = default;

    // 常用方法合集
    // 返回指向vector内存首地址的指针
    inline double * data() {
        return data_.data();
    }
    // at算符
    inline double & at(int64_t i, int64_t j) {
        return data_[i * col_ + j];
    }
    // 读取行数
    inline int64_t rows() {
        return row_;
    }
    // 读取列数
    inline int64_t cols() {
        return col_;
    }
    // 元素清零
    void zero();
    // 均匀随机初始化
    void uniform(double);
    // 计算该向量的标量乘法
    void scalerMul(double a);
    // 计算第i行的摸长
    double l2NormRow(int64_t i);
    // 计算一个和该向量第i行的内积
    double dotRow(Vector& source, int64_t, double);
    // 计算将外部向量加到某一行上,实现参数更新的效果
    void addVectorToRow(Vector& vec, int64_t i, double a);
    // 计算将某一行加到外部向量上,实现取出某一行的效果
    void addRowToVector(Vector& vec, int64_t i, double a);
    // 计算矩阵间的加法
    void addMatrix(Matrix& mat, double a);
};