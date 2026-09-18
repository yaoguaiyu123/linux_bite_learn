#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>

#define SEM_NAME "/posix_sem_demo"

int main(void)
{
    /*
     * 不使用O_CREAT，只打开已经存在的信号量。
     * 因此需要先运行sem_waiter。
     */
    sem_t *sem = sem_open(
        SEM_NAME,
        0
    );

    if (sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    printf("成功打开信号量\n");
    printf("准备发送通知\n");

    if (sem_post(sem) == -1) {
        perror("sem_post");
        sem_close(sem);
        return 1;
    }

    printf("通知已经发送\n");

    if (sem_close(sem) == -1) {
        perror("sem_close");
        return 1;
    }

    return 0;
}