#pragma once

#include <iostream>
#include <vector>

class Matrix {
public:
    Matrix(int rows, int cols);
    Matrix(const std::vector<std::vector<float>>& data);
    Matrix(const Matrix& other_matrix);
    Matrix();
    int getRows() const;
    int getCols() const;
    Matrix add(Matrix& first_matrix, Matrix& second_matrix);
    Matrix operator+(Matrix other_matrix);
    Matrix operator*(Matrix other_matrix);
    Matrix multiply(Matrix& first_matrix, Matrix& second_matrix);
    Matrix relu(const Matrix& other_matrix);
    Matrix softMax();
    std::vector<std::vector<float>>& getData();
private:
   std::vector<std::vector<float>> matrix_data;
   int rows;
   int cols;
};


std::vector<float> softMax(const std::vector<float>& vec);