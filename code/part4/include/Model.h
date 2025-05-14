#pragma once

#include "Matrix.h"

class Model {
public:
    Model();
    Model(Matrix& input_first, Matrix& input_second, Matrix& input_third, Matrix& input_fourth);
    std::vector<float> forward(Matrix& input);
private:
    Matrix weight1, weight2, bias1, bias2;
};