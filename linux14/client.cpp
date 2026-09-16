#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

int main()
{
    // 1. 创建TCP socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("socket");
        return 1;
    }

    // 2. 准备目标服务器地址
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // 3. 主动连接服务器
    if (connect(socket_fd, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) == -1) {
        perror("connect");
        close(socket_fd);
        return 1;
    }

    std::cout << "连接服务器成功\n";

    // 4. 发送消息
    const char *message = "hello server";
    send(socket_fd, message, std::strlen(message), 0);

    // 5. 接收服务器回复
    char buffer[1024]{};
    ssize_t received = recv(socket_fd, buffer, sizeof(buffer) - 1, 0);
    if (received > 0) {
        buffer[received] = '\0';
        std::cout << "服务器回复：" << buffer << '\n';
    }

    // 6. 关闭socket
    close(socket_fd);
    return 0;
}