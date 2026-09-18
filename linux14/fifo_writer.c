#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    const char *fifo_path = "/tmp/my_fifo";

    if (argc != 2) {
        fprintf(
            stderr,
            "用法：%s <message>\n",
            argv[0]
        );
        return 1;
    }

    printf("正在连接FIFO...\n");
    fflush(stdout);

    /*
     * 默认阻塞：
     * 如果没有进程打开FIFO的读端，
     * open会在这里等待。
     */
    int fd = open(fifo_path, O_WRONLY);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    const char *message = argv[1];
    size_t total = strlen(message);
    size_t sent = 0;

    while (sent < total) {
        ssize_t count = write(
            fd,
            message + sent,
            total - sent
        );

        if (count == -1) {
            if (errno == EINTR) {
                continue;
            }

            perror("write");
            close(fd);
            return 1;
        }

        sent += (size_t)count;
    }

    printf("成功写入 %zu 字节\n", sent);

    /*
     * 写端关闭后，如果已经没有其他写进程，
     * 读取端的read最终会返回0。
     */
    close(fd);
    return 0;
}