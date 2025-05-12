#pragma once

#include "Matrix_template.h"

class ModelBase {
public:
    virtual std::vector<float> forward(Matrix<float>& input) = 0;
    virtual std::vector<double> forward(Matrix<double>& input) = 0;
    virtual ~ModelBase() = default;
};

template <class T>
class Model : public ModelBase {
public:
    Model();
    Model(Matrix<T>& w1, Matrix<T>& b1, Matrix<T>& w2, Matrix<T>& b2);

    std::vector<T> forward_impl(Matrix<T>& input);

    std::vector<float> forward(Matrix<float>& input) override;
    std::vector<double> forward(Matrix<double>& input) override;
private:
    Matrix<T> weight1, weight2, bias1, bias2;
};