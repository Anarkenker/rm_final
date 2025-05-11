#ifndef _TCP_SOCKET_H_
#define _TCP_SOCKET_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <iostream>

#define PORT 8888

// 打印错误信息
#define err(errMsg) do { \
    std::cerr << "[line: " << __LINE__ << "] " << errMsg << " failed: " << errno << " (" << strerror(errno) << ")" << std::endl; \
} while(0)

// 初始化 socket
bool InitSocket();

// 关闭 socket
bool CloseSocket();

// 创建服务器 socket
int CreatServerSocket();

// 创建客户端 socket
int CreatClientSocket(const char* IP);

#endif