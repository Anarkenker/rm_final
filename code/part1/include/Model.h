#pragma once

#include "Matrix.h"

class Model {
public:
    Model();
    Model(Matrix& input_first, Matrix& input_second, Matrix& input_third, Matrix& input_fourth);
    std::vector<float> forward(Matrix& input);
    int getWeight1Rows() const { return weight1.getRows(); }
    int getWeight1Cols() const { return weight1.getCols(); }
    
private:
    Matrix weight1, weight2, bias1, bias2;
};