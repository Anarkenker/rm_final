#include "tcpSocket/tcpSocket.h"
#include "../part1/include/Matrix.h"
#include "../part1/include/Model.h"
#include <iostream>
#include <vector>

int main() {
    if (!InitSocket()) {
        std::cerr << "网络初始化失败！" << std::endl;
        return -1;
    }

    int server_fd = CreatServerSocket();
    if (server_fd == -1) {
        std::cerr << "创建服务器 socket 失败！" << std::endl;
        return -1;
    }
    std::cout << "等待客户端连接..." << std::endl;

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd == -1) {
        err("accept");
        close(server_fd);
        CloseSocket();
        return -1;
    }
    std::cout << "客户端已连接！" << std::endl;

    // 接收维度
    int rows = 0, cols = 0;
    recv(client_fd, &rows, sizeof(int), 0);
    recv(client_fd, &cols, sizeof(int), 0);

    // 接收矩阵数据
    std::vector<std::vector<float>> data(rows, std::vector<float>(cols));
    for (int i = 0; i < rows; ++i) {
        recv(client_fd, reinterpret_cast<char*>(data[i].data()), cols * sizeof(float), 0);
    }

    Matrix matrix_input(data);

    // 构造模型（目前是空矩阵）
    Matrix w1(784, 500), b1(1, 500), w2(500, 10), b2(1, 10);
    Model model(w1, b1, w2, b2);

    // 推理
    std::vector<float> result = model.forward(matrix_input);

    // 返回结果
    int len = result.size();
    send(client_fd, &len, sizeof(int), 0);
    send(client_fd, reinterpret_cast<const char*>(result.data()), len * sizeof(float), 0);

    close(client_fd);
    close(server_fd);
    CloseSocket();

    std::cout << "服务端结束。" << std::endl;
    return 0;
}