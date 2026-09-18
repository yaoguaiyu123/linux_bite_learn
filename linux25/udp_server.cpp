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

    // 绑定本地地址127.0.0.1:8080
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (bind(socket_fd, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) == -1) {
        perror("bind");
        close(socket_fd);
        return 1;
    }

    std::cout << "UDP服务端等待数据...\n";

    // recvfrom会同时返回数据和发送方地址
    char buffer[1024]{};
    sockaddr_in client_addr{};
    socklen_t client_addr_len = sizeof(client_addr);

    ssize_t received = recvfrom(
        socket_fd,
        buffer,
        sizeof(buffer) - 1,
        0,
        reinterpret_cast<sockaddr *>(&client_addr),
        &client_addr_len
    );

    if (received == -1) {
        perror("recvfrom");
        close(socket_fd);
        return 1;
    }

    buffer[received] = '\0';

    char client_ip[INET_ADDRSTRLEN]{};
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));

    std::cout << "收到客户端 " << client_ip << ':'
              << ntohs(client_addr.sin_port)
              << " 的消息：" << buffer << '\n';

    // 根据recvfrom得到的客户端地址回复
    const char *reply = "hello UDP client";

    if (sendto(
            socket_fd,
            reply,
            std::strlen(reply),
            0,
            reinterpret_cast<sockaddr *>(&client_addr),
            client_addr_len
        ) == -1) {
        perror("sendto");
    }

    close(socket_fd);
    return 0;
}