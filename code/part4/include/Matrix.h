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
    std::vector<std::vector<float>>& getData();
    void changeDeta(std::vector<std::vector<float>>& data) {
        this->matrix_data = data;
    }
private:
   std::vector<std::vector<float>> matrix_data;
   int rows;
   int cols;
};

struct task {
    Matrix* first_matrix;
    Matrix* second_matrix;
    Matrix* result_matrix;
    int cols;
    int start_row;
    int end_row;
    int common;
};

std::vector<float> softMax(const std::vector<float>& vec);

void threadMultiply(task& task_now);