#include "../include/model.h"

Model::Model() {
    weight1 = Matrix(784, 500);
    bias1 = Matrix(1, 500);
    weight2 = Matrix(500, 10);
    bias2 = Matrix(1, 10);
}


std::vector<float> Model::forward(Matrix& input) {
    Matrix temp = (input * weight1);
    for(int i = 0; i < 500; i++) {
        temp.getData()[0][i] = temp.getData()[0][i] + bias1.getData()[0][i];
    }
    temp = temp.relu(temp);
    temp = temp * weight2;
    for(int i = 0; i < 10; i++) {
        temp.getData()[0][i] = temp.getData()[0][i] + bias2.getData()[0][i];
    }
    Matrix result(1, 10);
    for(int i = 0; i < 10; i++) {
        result.getData()[0][i] = temp.getData()[0][i];
    }
    return softMax(result.getData()[0]);
}