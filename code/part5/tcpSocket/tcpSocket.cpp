#include "tcpSocket.h"

bool InitSocket() {
    return true;
}

bool CloseSocket(int socket_fd) {
    return close(socket_fd) == 0;
}

int CreatServerSocket() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        err("socket");
        return -1;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        err("setsockopt");
        close(server_fd);
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        err("bind");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 10) < 0) {
        err("listen");
        close(server_fd);
        return -1;
    }

    return server_fd;
}

int CreatClientSocket(const char* IP) {
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        err("socket");
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &addr.sin_addr);

    if (connect(client_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        err("connect");
        close(client_fd);
        return -1;
    }
    
    return client_fd;
}