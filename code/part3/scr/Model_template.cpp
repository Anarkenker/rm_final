#include "../include/Model_template.h"

template <class T>
Model<T>::Model() {
    weight1 = Matrix<T>(784, 500);
    bias1 = Matrix<T>(1, 500);
    weight2 = Matrix<T>(500, 10);
    bias2 = Matrix<T>(1, 10);
}

template <class T>
Model<T>::Model(Matrix<T>& w1, Matrix<T>& b1, Matrix<T>& w2, Matrix<T>& b2)
    : weight1(w1), bias1(b1), weight2(w2), bias2(b2) {}

template <class T>
std::vector<T> Model<T>::forward_impl(Matrix<T>& input) {
    Matrix<T> temp = input * weight1;
    for(int i = 0; i < 500; ++i) {
        temp.getData()[0][i] += bias1.getData()[0][i];
    }
    temp = temp.relu(temp);
    temp = temp * weight2;
    for(int i = 0; i < 10; ++i) {
        temp.getData()[0][i] += bias2.getData()[0][i];
    }
    return softMax(temp.getData()[0]);
}

template <>
std::vector<float> Model<float>::forward(Matrix<float>& input) {
    return forward_impl(input);
}

template <>
std::vector<double> Model<double>::forward(Matrix<double>& input) {
    return forward_impl(input);
}

template <>
std::vector<double> Model<float>::forward(Matrix<double>& input) {
    throw std::runtime_error("Model<float> 不支持 double 输入");
}

template <>
std::vector<float> Model<double>::forward(Matrix<float>& input) {
    throw std::runtime_error("Model<double> 不支持 float 输入");
}

template class Model<float>;
template class Model<double>;