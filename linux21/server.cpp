// note epoll + 非阻塞socket + ET模式
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <cstring>

using namespace std;

// 把 fd 设成非阻塞模式
// ET模式下必须采用非阻塞模式，不然recv会阻塞等待,导致线程卡死
int setNonBlock(int fd) {
    int flag = fcntl(fd, F_GETFL);
    flag |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flag);
}

void test09() {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(listen_fd, (sockaddr*)&addr, sizeof(addr));
    listen(listen_fd, 5);
    setNonBlock(listen_fd);   //设置为非阻塞模式

    int epfd = epoll_create(1);
    epoll_event ev{};
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listen_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);

    epoll_event events[10];
    while (true) {
        int n = epoll_wait(epfd, events, 10, -1);
        for (int i = 0; i < n; i++) {
            int fd = events[i].data.fd;
            if (fd == listen_fd) {
                while (true) {
                    int client_fd = accept(listen_fd, NULL, NULL);
                    if (client_fd == -1) break;
                    cout << "new client:" << client_fd << endl;
                    setNonBlock(client_fd);
                    epoll_event client_ev{};
                    client_ev.events = EPOLLIN | EPOLLET;
                    client_ev.data.fd = client_fd;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &client_ev);
                }
            } else if (events[i].events & EPOLLIN) {
                while (true) {
                    char buf[1024] = {0};
                     // 这边的recv函数被放在一个while循环里面，确保读完所有数据，也是必须采用非阻塞的原因
                    int ret = recv(fd, buf, sizeof(buf), 0); 
                    if (ret > 0) {
                        cout << "recv:" << buf << endl;
                        send(fd, buf, strlen(buf), 0);
                    } else {
                        if (ret == 0) {
                            cout << "client close" << endl;
                            close(fd);
                            epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                        }
                        break;
                    }
                }
            }
        }
    }
    close(listen_fd);
    close(epfd);
}

int main() {
    test09();
    return 0;
}