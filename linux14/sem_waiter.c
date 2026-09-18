#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>

#define SEM_NAME "/posix_sem_demo"

int main(void)
{
    /*
     * 删除可能由上一次程序异常退出留下的信号量。
     * 第一次运行时不存在是正常情况。
     */
    if (sem_unlink(SEM_NAME) == -1 &&
        errno != ENOENT) {
        perror("sem_unlink");
        return 1;
    }

    /*
     * O_CREAT：创建信号量
     * O_EXCL：如果已经存在则失败
     * 初始值为0
     */
    sem_t *sem = sem_open(
        SEM_NAME,
        O_CREAT | O_EXCL,
        0666,
        0
    );

    if (sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    printf("信号量已经创建，初始值为0\n");
    printf("等待另一个进程发送通知...\n");
    fflush(stdout);

    /*
     * 当前信号量值为0，因此会阻塞。
     */
    while (sem_wait(sem) == -1) {
        if (errno == EINTR) {
            continue;
        }

        perror("sem_wait");
        sem_close(sem);
        sem_unlink(SEM_NAME);
        return 1;
    }

    printf("收到通知，sem_wait成功返回\n");

    // 关闭当前进程的信号引用
    if (sem_close(sem) == -1) {
        perror("sem_close");
        sem_unlink(SEM_NAME);
        return 1;
    }

    // 删除信号名字
    if (sem_unlink(SEM_NAME) == -1) {
        perror("sem_unlink");
        return 1;
    }

    printf("信号量已经删除\n");
    return 0;
}