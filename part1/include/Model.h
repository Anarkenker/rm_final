#pragma once

#include "Matrix.h"

class Model {
public:
    Model();
    std::vector<float> forward(Matrix& input);
private:
    Matrix weight1, weight2, bias1, bias2;
};