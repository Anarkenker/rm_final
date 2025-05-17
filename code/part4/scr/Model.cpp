#include "../include/Model.h"
#include <chrono>
Model::Model() {
    weight1 = Matrix(784, 500);
    bias1 = Matrix(1, 500);
    weight2 = Matrix(500, 10);
    bias2 = Matrix(1, 10);
}

Model::Model(Matrix& input_first, Matrix& input_second, Matrix& input_third, Matrix& input_fourth) {
    weight1 = input_first;
    bias1 = input_second;
    weight2 = input_third;
    bias2 = input_fourth;
}

std::vector<float> Model::forward(Matrix& input) {
    auto start = std::chrono::high_resolution_clock::now();

    Matrix temp = (input * weight1);
    auto& temp_data = temp.getData();
    auto& bias1_data = bias1.getData();
    for (int i = 0; i < 500; i++) {
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

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "耗时：" << duration.count() << "ms" << std::endl;

    return softMax(result.getData()[0]);
}