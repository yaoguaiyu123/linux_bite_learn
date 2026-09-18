#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

int main()
{
    // 创建UDP socket
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1) {
        perror("socket");
        return 1;
    }

    // 设置目标服务端地址
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // 不需要connect，直接指定目标地址发送
    const char *message = "hello UDP server";

    if (sendto(
            socket_fd,
            message,
            std::strlen(message),
            0,
            reinterpret_cast<sockaddr *>(&server_addr),
            sizeof(server_addr)
        ) == -1) {
        perror("sendto");
        close(socket_fd);
        return 1;
    }

    std::cout << "消息已发送\n";

    // 等待服务端回复
    char buffer[1024]{};
    sockaddr_in reply_addr{};
    socklen_t reply_addr_len = sizeof(reply_addr);

    ssize_t received = recvfrom(
        socket_fd,
        buffer,
        sizeof(buffer) - 1,
        0,
        reinterpret_cast<sockaddr *>(&reply_addr),
        &reply_addr_len
    );

    if (received == -1) {
        perror("recvfrom");
        close(socket_fd);
        return 1;
    }

    buffer[received] = '\0';
    std::cout << "服务端回复：" << buffer << '\n';

    close(socket_fd);
    return 0;
}