#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "../part1/include/Model.h"
#include "../part5/tcpSocket/tcpSocket.h"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <string>


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
void processAndSendImage(const std::string& imagePath, int client_fd, Model& model) {
    cv::Mat img = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);
    if (img.empty()) {
        std::cerr << "无法读取图片: " << imagePath << std::endl;
        return;
    }

    cv::resize(img, img, cv::Size(28, 28));

    std::vector<float> inputVector(784);
    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            inputVector[i * 28 + j] = img.at<uchar>(i, j) / 255.0f;
        }
    }

    if (client_fd != -1) {
        int rows = htonl(1);
        int cols = htonl(inputVector.size());
        send(client_fd, &rows, sizeof(int), 0);
        send(client_fd, &cols, sizeof(int), 0);

        std::vector<float> network_data(inputVector.size());
        for (size_t i = 0; i < inputVector.size(); ++i) {
            float val = inputVector[i];
            char* p = reinterpret_cast<char*>(&val);
            float network_val;
            char* q = reinterpret_cast<char*>(&network_val);
            q[0] = p[3]; q[1] = p[2]; q[2] = p[1]; q[3] = p[0];
            network_data[i] = network_val;
        }

        send(client_fd, reinterpret_cast<const char*>(network_data.data()), inputVector.size() * sizeof(float), 0);

        std::vector<float> output(10);
        int network_len;
        if (recv(client_fd, &network_len, sizeof(int), 0) != sizeof(int)) {
            std::cerr << "接收预测结果长度失败！" << std::endl;
            return;
        }
        int len = ntohl(network_len);

        std::vector<float> network_output(len);
        if (recv(client_fd, reinterpret_cast<char*>(network_output.data()), len * sizeof(float), 0) != len * sizeof(float)) {
            std::cerr << "接收预测结果数据失败！" << std::endl;
            return;
        }

        for (int i = 0; i < len; ++i) {
            float val = network_output[i];
            char* p = reinterpret_cast<char*>(&val);
            float host_val;
            char* q = reinterpret_cast<char*>(&host_val);
            q[0] = p[3]; q[1] = p[2]; q[2] = p[1]; q[3] = p[0];
            output[i] = host_val;
        }

        std::cout << "预测结果: " << std::endl;
        int num = 0;
        for (float val : output) {
            std::cout << num << " : " << val << std::endl;
            num++;
        }
        std::cout << std::endl;

        int max_index = 0;
        float max_value = output[0];
        for (int i = 1; i < output.size(); ++i) {
            if (output[i] > max_value) {
                max_value = output[i];
                max_index = i;
            }
        }
        std::cout << "数字是: " << max_index << std::endl;
    } 
    else {
        Matrix inputMatrix({inputVector});
        std::vector<float> output = model.forward(inputMatrix);

        int num = 0;
        for (float val : output) {
            std::cout << num << " : " << val << std::endl;
            num++;
        }
        std::cout << std::endl;

        int max_index = 0;
        float max_value = output[0];
        for (int i = 1; i < output.size(); ++i) {
            if (output[i] > max_value) {
                max_value = output[i];
                max_index = i;
            }
        }
        std::cout << "数字是: " << max_index << std::endl;
    }
}

int main() {
    try {
        std::string numPath;
        std::cout << "输入你要检验的数字；" << std::endl;
        std::cin >> numPath;
        std::string imagePath = "../nums/" + numPath + ".png";
        Model model = readFile();

        int client_fd = CreatClientSocket("127.0.0.1");
        processAndSendImage(imagePath, client_fd, model);
        CloseSocket(client_fd);
    } 
    catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
    }

    return 0;
}