#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

#define SHM_NAME "/posix_shm_demo"

typedef struct {
    int number;
    char message[256];
} SharedData;

int main(void)
{
    int fd = shm_open(
        SHM_NAME,
        O_RDONLY,
        0
    );

    if (fd == -1) {
        perror("shm_open");
        return 1;
    }

    SharedData *data = mmap(
        NULL,
        sizeof(SharedData),
        PROT_READ,
        MAP_SHARED,
        fd,
        0
    );

    if (data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    close(fd);

    printf("读取共享内存\n");
    printf("number  = %d\n", data->number);
    printf("message = %s\n", data->message);
    printf("当前进程中的映射地址：%p\n", (void *)data);

    if (munmap(data, sizeof(SharedData)) == -1) {
        perror("munmap");
        return 1;
    }

    /*
     * 读取完成后删除共享内存名字。
     */
    if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink");
        return 1;
    }

    printf("共享内存已经删除\n");
    return 0;
}