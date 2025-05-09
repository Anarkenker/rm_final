#include "../include/Matrix.h"
#include <cmath>
Matrix::Matrix(int rows, int cols) {
    this->rows = rows;
    this->cols = cols;
    this->matrix_data = std::vector<std::vector<float>>(rows, std::vector<float>(cols));
}
Matrix::Matrix(const Matrix& other_matrix) {
    this->rows = other_matrix.getRows();
    this->cols = other_matrix.getCols();
    this->matrix_data = other_matrix.matrix_data;
}

Matrix::Matrix(const std::vector<std::vector<float>>& data) {
    this->rows = data.size();
    this->cols = data[0].size();
    this->matrix_data = data;
}

Matrix::Matrix() {
    this->rows = 10;
    this->cols = 10;
    this->matrix_data = std::vector<std::vector<float>>(10, std::vector<float>(10));
}
int Matrix::getRows() const {
    return this->rows;
}

int Matrix::getCols() const {
    return this->cols;
}

Matrix Matrix::add(Matrix& first_matrix, Matrix& second_matrix) { 
    Matrix result(first_matrix.getRows(), first_matrix.getCols());
    for(int i = 0; i < first_matrix.getRows(); i++) {
        for(int j = 0; j < first_matrix.getCols(); j++) {
            result.matrix_data[i][j] = first_matrix.matrix_data[i][j] + second_matrix.matrix_data[i][j];
        }
    }
    return result;
}

Matrix Matrix::operator+(Matrix other_matrix) {
    return Matrix::add(*this, other_matrix);
}

Matrix Matrix::operator*(Matrix other_matrix) {
    return Matrix::multiply(*this, other_matrix);
}

Matrix Matrix::multiply(Matrix& first_matrix, Matrix& second_matrix) {
    Matrix result(first_matrix.getRows(), second_matrix.getCols());
    for (int i = 0; i < first_matrix.getRows(); ++i) {
        for (int j = 0; j < second_matrix.getCols(); ++j) {
            for (int k = 0; k < first_matrix.getCols(); ++k) {
                result.matrix_data[i][j] += first_matrix.matrix_data[i][k] * second_matrix.matrix_data[k][j];
            }
        }
    }
    return result;
}

Matrix Matrix::relu(const Matrix& other_matrix) {
    Matrix result(other_matrix.getRows(), other_matrix.getCols());
    for (int i = 0; i < other_matrix.getRows(); ++i) {
        for (int j = 0; j < other_matrix.getCols(); ++j) {
            result.matrix_data[i][j] = other_matrix.matrix_data[i][j] > 0 ? other_matrix.matrix_data[i][j] : 0;
        }
    }
    return result;
}

std::vector<float> softMax(const std::vector<float>& vec) {
    std::vector<float> result(vec.size());
    float sum = 0.0f;

    for (float val : vec) {
        sum += exp(val);
    }

    for (size_t i = 0; i < vec.size(); ++i) {
        result[i] = exp(vec[i]) / sum;
    }
    return result;
}

std::vector<std::vector<float>>& Matrix::getData() {
    return matrix_data;
}