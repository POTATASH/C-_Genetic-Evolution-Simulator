#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <random>

template <typename T>
class Matrix {
public:
    // =========================================================================
    //  Constructors & Static Factories
    // =========================================================================

    Matrix(int rows, int cols)
        : m_rows(rows), m_cols(cols), m_data(rows * cols, static_cast<T>(0)) {
        if (rows <= 0 || cols <= 0) {
            throw std::invalid_argument("Matrix dimensions must be positive.");
        }
    }

    // Fix: Static method must be inside the header for templates[cite: 18, 19]
    static Matrix<T> fromVector(const std::vector<T>& vec) {
        Matrix<T> m(static_cast<int>(vec.size()), 1);
        for (int i = 0; i < static_cast<int>(vec.size()); ++i) {
            m.at(i, 0) = vec[i];
        }
        return m;
    }

    // =========================================================================
    //  Accessors
    // =========================================================================

    // Fix: Added toVector to return the flat data[cite: 18, 19]
    std::vector<T> toVector() const {
        return m_data;
    }

    T& at(int r, int c) {
        return m_data[r * m_cols + c];
    }

    const T& at(int r, int c) const {
        return m_data[r * m_cols + c];
    }

    int rows() const { return m_rows; }
    int cols() const { return m_cols; }

    // =========================================================================
    //  Operations
    // =========================================================================

    // Fix: Moved randomize into the header[cite: 18]
    void randomize(T low, T high) {
        static thread_local std::mt19937 rng{ std::random_device{}() };
        std::uniform_real_distribution<T> dist(low, high);
        for (auto& val : m_data) {
            val = dist(rng);
        }
    }

    void apply(const std::function<T(T)>& func) {
        for (auto& val : m_data) {
            val = func(val);
        }
    }

    // =========================================================================
    //  Operator Overloads
    // =========================================================================

    Matrix<T> operator+(const Matrix<T>& other) const {
        if (m_rows != other.m_rows || m_cols != other.m_cols) {
            throw std::runtime_error("Matrix dimensions must match for addition.");
        }
        Matrix<T> result(m_rows, m_cols);
        for (size_t i = 0; i < m_data.size(); ++i) {
            result.m_data[i] = m_data[i] + other.m_data[i];
        }
        return result;
    }

    Matrix<T> operator*(const Matrix<T>& other) const {
        if (m_cols != other.m_rows) {
            throw std::runtime_error("Matrix dimensions mismatch for multiplication.");
        }
        Matrix<T> result(m_rows, other.m_cols);
        for (int i = 0; i < m_rows; ++i) {
            for (int j = 0; j < other.m_cols; ++j) {
                T sum = 0;
                for (int k = 0; k < m_cols; ++k) {
                    sum += this->at(i, k) * other.at(k, j);
                }
                result.at(i, j) = sum;
            }
        }
        return result;
    }

private:
    int m_rows;
    int m_cols;
    std::vector<T> m_data; // Internal storage for the template[cite: 18]
};

#endif