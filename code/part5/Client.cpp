#include "tcpSocket.h"
#include <iostream>
#include <vector>

int main() {
    int client_fd = CreatClientSocket("127.0.0.1");
    if (client_fd == -1) {
        std::cerr << "连接失败！" << std::endl;
        return -1;
    }

    int rows = 0, cols = 0;
    std::cin >> rows >> cols;
    send(client_fd, &rows, sizeof(int), 0);
    send(client_fd, &cols, sizeof(int), 0);

    std::vector<float> input(784, 1.0f);
    send(client_fd, input.data(), input.size() * sizeof(float), 0);

    int result_size;
    recv(client_fd, &result_size, sizeof(int), 0);

    std::vector<float> result(result_size);
    recv(client_fd, result.data(), result_size * sizeof(float), 0);

    for (int i = 0; i < result_size; ++i) {
        std::cout << result[i] << std::endl;
    }

    close(client_fd);
    return 0;
}