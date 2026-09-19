// note poll IO多路复用
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <cstring>

using namespace std;

void test_poll_server() {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(listen_fd, (sockaddr*)&addr, sizeof(addr));
    listen(listen_fd, 5);   // 开始监听


    // tip 结构体
    // struct pollfd {
    //     int   fd;       // 监控哪个文件描述符
    //     short events;   // 关心什么事件（输入）
    //     short revents;  // 实际发生了什么事件（输出，内核填的）
    // };

    pollfd fds[10];
    for (int i = 0; i < 10; i++) fds[i].fd = -1;
    fds[0].fd = listen_fd;   // 监听 listen_fd
    fds[0].events = POLLIN;  // 指定事件为 POLLIN:这个 fd 上有数据可读的时候通知我

    while (true) {
        // 阻塞等待，直到有事件发生
        int ret = poll(fds, 10, -1);
        if (ret < 0) break;

        if (fds[0].revents & POLLIN) {  // 有新连接
            int client_fd = accept(listen_fd, NULL, NULL);  // 得到客户端对应的 fd 描述符
            cout << "new client:" << client_fd << endl;
            for (int i = 1; i < 10; i++) {
                if (fds[i].fd == -1) {
                    fds[i].fd = client_fd;
                    fds[i].events = POLLIN;
                    break;
                }
            }
        }

        for (int i = 1; i < 10; i++) {
            if (fds[i].fd != -1 && fds[i].revents & POLLIN) {   // 接收到新的消息
                char buf[1024] = {0};
                int n = recv(fds[i].fd, buf, sizeof(buf), 0);
                if (n <= 0) {
                    close(fds[i].fd);
                    fds[i].fd = -1;
                    cout << "client close" << endl;
                } else {
                    cout << "recv:" << buf << endl;
                    send(fds[i].fd, buf, strlen(buf), 0);
                }
            }
        }
    }
    close(listen_fd);
}

int main() {
    test_poll_server();
    return 0;
}