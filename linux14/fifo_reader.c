#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

int main(void)
{
    const char *fifo_path = "/tmp/my_fifo";

    /*
     * 创建FIFO。
     * 如果FIFO已经存在，mkfifo会失败并设置errno=EEXIST，
     * 这种情况可以继续使用已有的FIFO。
     */
    if (mkfifo(fifo_path, 0666) == -1 &&
        errno != EEXIST) {
        perror("mkfifo");
        return 1;
    }

    printf("FIFO已经准备好：%s\n", fifo_path);
    printf("等待写进程连接...\n");
    fflush(stdout);

    /*
     * 默认阻塞：
     * 如果暂时没有进程打开FIFO的写端，
     * open会在这里等待。
     */
    int fd = open(fifo_path, O_RDONLY);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    printf("写进程已经连接\n");

    char buffer[128];

    while (1) {
        ssize_t count = read(
            fd,
            buffer,
            sizeof(buffer) - 1
        );

        if (count == -1) {
            if (errno == EINTR) {
                continue;
            }

            perror("read");
            break;
        }

        if (count == 0) {
            printf("所有写端都已关闭，读取到EOF\n");
            break;
        }

        buffer[count] = '\0';

        printf(
            "读取到 %zd 字节：%s\n",
            count,
            buffer
        );
    }

    close(fd);

    /*
     * 删除FIFO的文件系统名字。
     * 如果还要反复启动读取端，可以暂时不删除。
     */
    if (unlink(fifo_path) == -1) {
        perror("unlink");
        return 1;
    }

    printf("FIFO已经删除\n");
    return 0;
}