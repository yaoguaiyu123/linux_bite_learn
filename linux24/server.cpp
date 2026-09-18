
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

int main()
{
    // 1. 创建TCP socket，返回文件描述符, SOCK_STREAM 表示 TCP
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        perror("socket");
        return 1;
    }

    // 允许程序关闭后快速重新绑定8080端口
    int reuse = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // 2. 准备服务端地址：127.0.0.1:8080
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // 3. 把socket绑定到IP和端口
    if (bind(listen_fd, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) == -1) {
        perror("bind");
        close(listen_fd);
        return 1;
    }

    // 4. 进入监听状态
    if (listen(listen_fd, 5) == -1) {
        perror("listen");
        close(listen_fd);
        return 1;
    }

    std::cout << "等待客户端连接 127.0.0.1:8080...\n";

    // 5. 等待客户端连接
    sockaddr_in client_addr{};
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = accept(listen_fd, reinterpret_cast<sockaddr *>(&client_addr), &client_addr_len);
    if (client_fd == -1) {
        perror("accept");
        close(listen_fd);
        return 1;
    }

    std::cout << "客户端已连接\n";

    // 6. 接收客户端消息
    char buffer[1024]{};
    ssize_t received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (received > 0) {
        buffer[received] = '\0';
        std::cout << "收到：" << buffer << '\n';
    }

    // 7. 给客户端回复
    const char *reply = "hello client";
    send(client_fd, reply, std::strlen(reply), 0);

    // 8. 关闭两个socket
    close(client_fd);
    close(listen_fd);
    return 0;
}