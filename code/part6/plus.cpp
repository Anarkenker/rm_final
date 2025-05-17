#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include "../part1/include/Model.h"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <string>
#include <filesystem>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../part5/tcpSocket/tcpSocket.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

cv::Mat canvas(280, 280, CV_8UC1, cv::Scalar(255));
std::atomic<bool> running(true);
std::vector<float> output(10, 0.0f);
std::atomic<int> predictedDigit(-1);

cv::Scalar colors[10] = {
    cv::Scalar(255, 0, 0),
    cv::Scalar(0, 255, 0), 
    cv::Scalar(0, 0, 255),  
    cv::Scalar(255, 255, 0),  
    cv::Scalar(255, 0, 255),  
    cv::Scalar(0, 255, 255),  
    cv::Scalar(128, 0, 128),  
    cv::Scalar(128, 128, 0),  
    cv::Scalar(0, 128, 128),  
    cv::Scalar(128, 128, 128) 
};

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

void drawCallback(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_LBUTTONDOWN || (flags & cv::EVENT_FLAG_LBUTTON)) {
        cv::circle(canvas, cv::Point(x, y), 20, cv::Scalar(0), -1);
    }
}

void predictDigit(int client_fd, Model& model) {
    while(running) {
        cv::Mat resize, blurred, img;
        cv::resize(canvas, resize, cv::Size(28, 28));
        cv::GaussianBlur(resize, blurred, cv::Size(5, 5), 0);
        cv::threshold(blurred, img, 128, 255, cv::THRESH_BINARY);
        
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
            predictedDigit = max_index;
        } 
        else {
            Matrix inputMatrix({inputVector});
            output = model.forward(inputMatrix);

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
            predictedDigit = max_index;
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
}


int main() {
    try {
        Model model = readFile();

        int client_fd = -1;

        try {
            client_fd = CreatClientSocket("127.0.0.1");
        } 
        catch (...) {
            client_fd = -1;
        }
        if (client_fd < 0) {
            std::cout << "未连接到server，使用本地模型推理。" << std::endl;
        } 
        else {
            std::cout << "已连接到server，使用server推理。" << std::endl;
        }

        cv::namedWindow("Draw Digit");
        cv::setMouseCallback("Draw Digit", drawCallback);

        std::thread predictionThread(predictDigit, client_fd, std::ref(model));

        while (running) {
            cv::Mat display = cv::Mat::zeros(280, 400, CV_8UC3);
            cv::cvtColor(canvas, display(cv::Rect(0, 0, 280, 280)), cv::COLOR_GRAY2BGR);

            if (predictedDigit >= 0) {
                cv::putText(display, "Predicted: " + std::to_string(predictedDigit), cv::Point(10, 30),
                            cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0), 2);
                int barHeightMax = 200;
                for (int i = 0; i < 10; ++i) {
                    int barHeight = static_cast<int>(output[i] * barHeightMax);
                    cv::rectangle(display, cv::Point(300, i * 20 + 10),
                                  cv::Point(300 + barHeight, i * 20 + 25), colors[i], -1);
                    cv::putText(display, std::to_string(i), cv::Point(280, i * 20 + 25),
                                cv::FONT_HERSHEY_SIMPLEX, 0.5, colors[i], 1);
                }
            }

            cv::imshow("Draw Digit", display);

            char key = cv::waitKey(1);
            if (key == 'q') running = false;
            else if (key == 'c') canvas.setTo(cv::Scalar(255));
        }

        predictionThread.join();
        if (client_fd >= 0) close(client_fd);
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
    }
    return 0;
}