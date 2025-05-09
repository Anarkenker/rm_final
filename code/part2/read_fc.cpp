#include "../part1/include/model.h"
#include "../part1/include/Matrix.h"
#include <iostream>
#include <vector>
#include <cstdio>
#include <fstream>
#include <string>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <nlohmann/json.hpp>
#include <filesystem>

using json = nlohmann::json;

Matrix read_fc(const std::string& file_name, int rows, int cols) {
    std::ifstream file(file_name, std::ios::binary);
    std::vector<std::vector<float>> data(rows, std::vector<float>(cols));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            float val;
            file.read(reinterpret_cast<char*>(&val), sizeof(float));
            data[i][j] = val;
        }
    }
    return Matrix(data);
}

int main() {
    std::ifstream read_file_meta("../mnist-fc/meta.json");
    if (!read_file_meta.is_open()) {
        std::cerr << "无法打开 meta.json 文件！" << std::endl;
        return 1;
    }
    json meta;
    read_file_meta >> meta;

    auto weight1_shape = meta["fc1.weight"];
    auto bias1_shape = meta["fc1.bias"];
    auto weight2_shape = meta["fc2.weight"];
    auto bias2_shape = meta["fc2.bias"];

    Matrix weight1 = read_fc("../mnist-fc/fc1.weight", weight1_shape[0], weight1_shape[1]);
    Matrix bias1   = read_fc("../mnist-fc/fc1.bias",   bias1_shape[0],  bias1_shape[1]);
    Matrix weight2 = read_fc("../mnist-fc/fc2.weight", weight2_shape[0], weight2_shape[1]);
    Matrix bias2   = read_fc("../mnist-fc/fc2.bias",   bias2_shape[0],  bias2_shape[1]);

    Model model(weight1, bias1, weight2, bias2);

    return 0;
}