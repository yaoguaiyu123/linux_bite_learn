#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

using namespace std;

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    connect(fd, (sockaddr*)&addr, sizeof(addr));

    while (true) {
        string msg;
        cin >> msg;
        send(fd, msg.c_str(), msg.size(), 0);

        char buf[1024] = {0};
        recv(fd, buf, sizeof(buf), 0);
        cout << "server:" << buf << endl;
    }

    close(fd);
    return 0;
}