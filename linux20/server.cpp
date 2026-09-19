// note Epoll IO多路复用
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <cstring>

using namespace std;

void test_epoll() {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(listen_fd, (sockaddr*)&addr, sizeof(addr));
    listen(listen_fd, 5);   //开始监听

    int epfd = epoll_create(1);   // 创建一个epoll实例，返回epoll的fd
    // struct epoll_event {
    //     uint32_t     events;    // 关心的事件
    //     epoll_data_t data;      // 附带的数据（通常放 fd）
    // };
    epoll_event ev{};  // 事件结构体
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;   // 绑定listen_fd
    epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);    // 把listen_fd添加到epoll

    epoll_event events[10];
    while (true) {
        int n = epoll_wait(epfd, events, 10, -1);
        for (int i = 0; i < n; i++) {
            int fd = events[i].data.fd;
            if (fd == listen_fd) {   // 新连接
                int client_fd = accept(listen_fd, NULL, NULL);
                cout << "new client:" << client_fd << endl;
                epoll_event client_ev{};
                client_ev.events = EPOLLIN;
                client_ev.data.fd = client_fd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &client_ev);
            } else {       // 旧连接发送消息
                char buf[1024] = {0};
                int ret = recv(fd, buf, sizeof(buf), 0);
                if (ret <= 0) {
                    cout << "client close" << endl;
                    close(fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);   // 删除client对应的fd
                } else {
                    cout << "recv:" << buf << endl;
                    send(fd, buf, strlen(buf), 0);
                }
            }
        }
    }
    close(listen_fd);
    close(epfd);
}

int main() {
    test_epoll();
    return 0;
}