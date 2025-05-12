#include "include/Model_template.h"
#include "include/Matrix_template.h"
#include <iostream>
#include <vector>
#include <cstdio>
#include <fstream>
#include <string>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

template <class T>
Matrix<T> readFc(const std::string& file_name, int rows, int cols) {
    std::ifstream file(file_name, std::ios::binary);
    std::vector<std::vector<T>> data(rows, std::vector<T>(cols));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            T val;
            file.read(reinterpret_cast<char*>(&val), sizeof(T));
            data[i][j] = val;
        }
    }
    return Matrix<T>(data);
}

template <class T>
ModelBase* returnFc(const std::string& file_name) {
    std::ifstream read_file_meta(file_name + "/meta.json");
    if (!read_file_meta.is_open()) {
        std::cerr << "无法打开 meta.json 文件！" << std::endl;
        return nullptr;
    }
    json meta;
    read_file_meta >> meta;

    auto weight1_shape = meta["fc1.weight"];
    auto bias1_shape = meta["fc1.bias"];
    auto weight2_shape = meta["fc2.weight"];
    auto bias2_shape = meta["fc2.bias"];

    Matrix<T> weight1 = readFc<T>(file_name + "/fc1.weight", weight1_shape[0], weight1_shape[1]);
    Matrix<T> bias1   = readFc<T>(file_name + "/fc1.bias",   bias1_shape[0],  bias1_shape[1]);
    Matrix<T> weight2 = readFc<T>(file_name + "/fc2.weight", weight2_shape[0], weight2_shape[1]);
    Matrix<T> bias2   = readFc<T>(file_name + "/fc2.bias",   bias2_shape[0],  bias2_shape[1]);

    return new Model<T>(weight1, bias1, weight2, bias2);
}
int main() {
    
    std::filesystem::path current_path = std::filesystem::current_path();
    std::cout << "当前工作路径: " << current_path.string() << std::endl;
    
    std::string type;
    std::cin >> type;
    ModelBase* model = nullptr;
    if(type == "plus") {
        model = returnFc<double>("../mnist-fc-plus/");
    }
    else {
        model = returnFc<float>("../mnist-fc/");
    }

    delete model;

    return 0;
}