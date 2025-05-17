#include "tcpSocket/tcpSocket.h"
#include "../part1/include/Matrix.h"
#include "../part1/include/Model.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <stdexcept>
#include <opencv2/opencv.hpp>


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

Model readFile() {
    std::filesystem::path current_path = std::filesystem::current_path();
    std::cout << "当前工作路径: " << current_path.string() << std::endl;

    std::ifstream read_file_meta("../mnist-fc/meta.json");
    if (!read_file_meta.is_open()) {
        throw std::runtime_error("无法打开 meta.json 文件！");
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

    return Model(weight1, bias1, weight2, bias2);
}
int main() {
    if (!InitSocket()) { std::cerr << "网络初始化失败！" << std::endl; return -1; }
    int server_fd = CreatServerSocket();
    if (server_fd == -1) { std::cerr << "创建服务器 socket 失败！" << std::endl; return -1; }
    Model model = readFile();

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while (true) {
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd == -1) continue;
        std::cout << "客户端已连接！" << std::endl;

        while (true) { 
            int rows = 0, cols = 0;
            if (recv(client_fd, &rows, sizeof(int), 0) <= 0 ||
                recv(client_fd, &cols, sizeof(int), 0) <= 0) {
                break;
            }
            rows = ntohl(rows);
            cols = ntohl(cols);

            std::vector<std::vector<float>> data(rows, std::vector<float>(cols));
            for (int i = 0; i < rows; ++i) {
                int bytes_to_recv = cols * sizeof(float);
                int received = recv(client_fd, reinterpret_cast<char*>(data[i].data()), bytes_to_recv, 0);
                if (received != bytes_to_recv) {
                    std::cerr << i << " 行数据失败！" << std::endl;
                    CloseSocket(client_fd);
                    continue;
                }
                for (int j = 0; j < cols; ++j) {
                    float network_val = data[i][j];
                    char* p = reinterpret_cast<char*>(&network_val);
                    float host_val;
                    char* q = reinterpret_cast<char*>(&host_val);
                    q[0] = p[3]; q[1] = p[2]; q[2] = p[1]; q[3] = p[0];
                    data[i][j] = host_val;
                }
            }

            Matrix matrix_input(data);

            std::vector<float> result = model.forward(matrix_input);

            int len = result.size();
            int network_len = htonl(len);
            send(client_fd, &network_len, sizeof(int), 0);

            std::vector<float> network_result(len);
            for (int i = 0; i < len; ++i) {
                float val = result[i];
                char* p = reinterpret_cast<char*>(&val);
                float network_val;
                char* q = reinterpret_cast<char*>(&network_val);
                q[0] = p[3]; q[1] = p[2]; q[2] = p[1]; q[3] = p[0];
                network_result[i] = network_val;
            }

            send(client_fd, network_result.data(), len * sizeof(float), 0);

            std::cout << "预测结果: ";
            for (float val : result) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
        CloseSocket(client_fd);
    }
    CloseSocket(server_fd);
    return 0;
}