#pragma once

#include <iostream>
#include <vector>

template <class T>
class Matrix {
public:
    Matrix(int rows, int cols);
    Matrix(const std::vector<std::vector<T>>& data);
    Matrix(const Matrix& other_matrix);
    Matrix();
    int getRows() const;
    int getCols() const;
    Matrix add(Matrix& first_matrix, Matrix& second_matrix);
    Matrix operator+(Matrix other_matrix);
    Matrix operator*(Matrix other_matrix);
    Matrix multiply(Matrix& first_matrix, Matrix& second_matrix);
    Matrix relu(const Matrix& other_matrix);
    std::vector<std::vector<T>>& getData();
private:
   std::vector<std::vector<T>> matrix_data;
   int rows;
   int cols;
};

template <class T>
std::vector<T> softMax(const std::vector<T>& vec);