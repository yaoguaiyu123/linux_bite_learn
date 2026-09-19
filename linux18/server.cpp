#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <cstring>

using namespace std;

void test_select_server() {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);  // TCP
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(listen_fd, (sockaddr*)&addr, sizeof(addr));  // 绑定端口
    listen(listen_fd, 5);   // 开始监听

    fd_set readfds;    // 一个位图，用来告诉 select 要监控哪些 fd,select 返回后，内核会修改这个位图
    int max_fd = listen_fd;   // select 需要知道监控的最大 fd 编号是多少
    static int clients[10] = {0};  // 一个数组，用来保存所有已连接客户端的 fd

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(listen_fd, &readfds);   // 监听listen_fd, 监听的是新客户端连接
        for (int i = 0; i < 10; i++) {
            if (clients[i] > 0) {
                FD_SET(clients[i], &readfds);   // 监听客户端fd, 监听的是客户端发送的数据
                if (clients[i] > max_fd)
                    max_fd = clients[i];
            }
        }

        cout << "waiting..." << endl;
        // tip 这里设置select()最后一个参数为NULL, 表示不设置超时时间，阻塞等待
        // ret 表示有 ret 个 fd 发生了事件
        int ret = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (ret < 0) break;

        // 新连接事件
        // FD_ISSET() 表示判断 listen_fd 是否在 readfds 中位图中
        if (FD_ISSET(listen_fd, &readfds)) {
            int client_fd = accept(listen_fd, NULL, NULL);
            cout << "new client:" << client_fd << endl;
            for (int i = 0; i < 10; i++) {
                if (clients[i] == 0) {
                    clients[i] = client_fd;
                    break;
                }
            }
        }

        // 客户端发送数据事件
        for (int i = 0; i < 10; i++) {
            int fd = clients[i];
            if (fd > 0 && FD_ISSET(fd, &readfds)) {
                char buf[1024] = {0};
                int n = recv(fd, buf, sizeof(buf), 0);
                if (n <= 0) {
                    close(fd);
                    clients[i] = 0;
                    cout << "client close" << endl;
                } else {
                    cout << "recv:" << buf << endl;
                    send(fd, buf, strlen(buf), 0);
                }
            }
        }
    }
}

int main() {
    test_select_server();
    return 0;
}