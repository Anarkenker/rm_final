#include "../include/Matrix_template.h"
#include <cmath>

template <class T>
Matrix<T>::Matrix(int rows, int cols) {
    this->rows = rows;
    this->cols = cols;
    this->matrix_data = std::vector<std::vector<T>>(rows, std::vector<T>(cols));
}

template <class T>
Matrix<T>::Matrix(const Matrix<T>& other_matrix) {
    this->rows = other_matrix.getRows();
    this->cols = other_matrix.getCols();
    this->matrix_data = other_matrix.matrix_data;
}

template<class T>
Matrix<T>::Matrix(const std::vector<std::vector<T>>& data) {
    this->rows = data.size();
    this->cols = data[0].size();
    this->matrix_data = data;
}

template <class T>
Matrix<T>::Matrix() {
    this->rows = 10;
    this->cols = 10;
    this->matrix_data = std::vector<std::vector<T>>(10, std::vector<T>(10));
}
template <class T>
int Matrix<T>::getRows() const {
    return this->rows;
}

template <class T>
int Matrix<T>::getCols() const {
    return this->cols;
}

template <class T>
Matrix<T> Matrix<T>::add(Matrix<T>& first_matrix, Matrix<T>& second_matrix) { 
    Matrix result(first_matrix.getRows(), first_matrix.getCols());
    for(int i = 0; i < first_matrix.getRows(); i++) {
        for(int j = 0; j < first_matrix.getCols(); j++) {
            result.matrix_data[i][j] = first_matrix.matrix_data[i][j] + second_matrix.matrix_data[i][j];
        }
    }
    return result;
}

template <class T>
Matrix<T> Matrix<T>::operator+(Matrix<T> other_matrix) {
    return Matrix<T>::add(*this, other_matrix);
}

template <class T>
Matrix<T> Matrix<T>::operator*(Matrix<T> other_matrix) {
    return Matrix<T>::multiply(*this, other_matrix);
}

template <class T>
Matrix<T> Matrix<T>::multiply(Matrix<T>& first_matrix, Matrix<T>& second_matrix) {
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

template <class T>
Matrix<T> Matrix<T>::relu(const Matrix<T>& other_matrix) {
    Matrix<T> result(other_matrix.getRows(), other_matrix.getCols());
    for (int i = 0; i < other_matrix.getRows(); ++i) {
        for (int j = 0; j < other_matrix.getCols(); ++j) {
            result.matrix_data[i][j] = other_matrix.matrix_data[i][j] > 0 ? other_matrix.matrix_data[i][j] : 0;
        }
    }
    return result;
}

template <class T>
std::vector<T> softMax(const std::vector<T>& vec) {
    std::vector<T> result(vec.size());
    T sum = 0;

    for (float val : vec) {
        sum += exp(val);
    }

    for (size_t i = 0; i < vec.size(); ++i) {
        result[i] = exp(vec[i]) / sum;
    }
    return result;
}

template <class T>
std::vector<std::vector<T>>& Matrix<T>::getData() {
    return matrix_data;
}


template class Matrix<float>;
template class Matrix<double>;
template std::vector<float> softMax(const std::vector<float>& vec);
template std::vector<double> softMax(const std::vector<double>& vec);