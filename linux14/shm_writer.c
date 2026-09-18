#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

// 共享内存的名字必须以 / 开头
#define SHM_NAME "/posix_shm_demo"

// 共享内存结构体，写进程和读进程必须设置得一样
typedef struct {
    int number;
    char message[256];
} SharedData;

int main(void)
{
    // 创建共享内存
    int fd = shm_open(
        SHM_NAME,
        O_CREAT | O_RDWR,
        0666
    );

    if (fd == -1) {
        perror("shm_open");
        return 1;
    }

    // 设置共享内存大小
    if (ftruncate(fd, sizeof(SharedData)) == -1) {
        perror("ftruncate");
        close(fd);
        shm_unlink(SHM_NAME);
        return 1;
    }

    // 映射到进程地址空间
    // NULL	让系统自动选择虚拟地址
    // sizeof(SharedData)	映射长度
    // PROT_READ | PROT_WRITE	允许读写
    // MAP_SHARED	修改对其他进程可见
    // fd	shm_open()返回的描述符
    // 0	从共享内存起始位置映射
    SharedData *data = mmap(
        NULL,
        sizeof(SharedData),
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd,
        0
    );

    if (data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        shm_unlink(SHM_NAME);
        return 1;
    }

    
    //  mmap成功后可以关闭fd,已建立的内存映射仍然有效
    // 因为 close(fd) 只是删除当前进程文件描述符表中的这一项
    close(fd);

    data->number = 42;

    snprintf(
        data->message,
        sizeof(data->message),
        "hello from writer"
    );

    printf("写入完成\n");
    printf("number  = %d\n", data->number);
    printf("message = %s\n", data->message);
    printf("当前进程中的映射地址：%p\n", (void *)data);

    if (munmap(data, sizeof(SharedData)) == -1) {
        perror("munmap");
        return 1;
    }

    /*
     * 这里暂时不调用shm_unlink()，
     * 让读取端之后还能通过名字打开共享内存。
     */
    return 0;
}