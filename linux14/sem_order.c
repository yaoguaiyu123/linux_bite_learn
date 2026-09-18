#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define PROCESS_COUNT 3
#define ROUND_COUNT 3

static const char *sem_names[PROCESS_COUNT] = {
    "/sem_order_a",
    "/sem_order_b",
    "/sem_order_c"
};

/*
 * sem_wait可能被信号中断。
 * errno为EINTR时重新等待。
 */
int wait_semaphore(sem_t *sem)
{
    while (sem_wait(sem) == -1) {
        if (errno == EINTR) {
            continue;
        }

        return -1;
    }

    return 0;
}

/*
 * 子进程执行函数。
 *
 * current_sem：当前进程等待的信号量
 * next_sem：执行完成后通知的下一个信号量
 */
void run_process(
    char process_name,
    sem_t *current_sem,
    sem_t *next_sem,
    sem_t *semaphores[]
)
{
    for (int round = 1;
         round <= ROUND_COUNT;
         ++round) {

        if (wait_semaphore(current_sem) == -1) {
            perror("sem_wait");
            _exit(1);
        }

        printf(
            "进程%c执行，第%d轮，pid=%d\n",
            process_name,
            round,
            getpid()
        );

        /*
         * 将下一个进程对应的信号量加1，
         * 从而唤醒下一个进程。
         */
        if (sem_post(next_sem) == -1) {
            perror("sem_post");
            _exit(1);
        }
    }

    /*
     * fork后，子进程继承了三个信号量的引用，
     * 因此退出前分别关闭。
     */
    for (int i = 0; i < PROCESS_COUNT; ++i) {
        sem_close(semaphores[i]);
    }

    _exit(0);
}

int main(void)
{
    /*
     * 禁用stdout缓冲，确保每次printf立即显示，
     * 避免fork与缓冲造成干扰。
     */
    setbuf(stdout, NULL);

    sem_t *semaphores[PROCESS_COUNT];
    pid_t children[PROCESS_COUNT];
    int initial_values[PROCESS_COUNT] = {1, 0, 0};

    /*
     * 删除上一次异常退出可能遗留的有名信号量。
     */
    for (int i = 0; i < PROCESS_COUNT; ++i) {
        if (sem_unlink(sem_names[i]) == -1 &&
            errno != ENOENT) {
            perror("sem_unlink");
            return 1;
        }
    }

    /*
     * 创建三个信号量。
     */
    for (int i = 0; i < PROCESS_COUNT; ++i) {
        semaphores[i] = sem_open(
            sem_names[i],
            O_CREAT | O_EXCL,
            0666,
            initial_values[i]
        );

        if (semaphores[i] == SEM_FAILED) {
            perror("sem_open");

            for (int j = 0; j < i; ++j) {
                sem_close(semaphores[j]);
                sem_unlink(sem_names[j]);
            }

            return 1;
        }
    }

    printf("三个信号量创建完成\n");
    printf("初始值：A=1，B=0，C=0\n\n");

    /*
     * 创建三个子进程。
     */
    for (int i = 0; i < PROCESS_COUNT; ++i) {
        children[i] = fork();

        if (children[i] == -1) {
            perror("fork");

            /*
             * 如果创建子进程失败，终止已经创建的子进程。
             */
            for (int j = 0; j < i; ++j) {
                kill(children[j], SIGTERM);
                waitpid(children[j], NULL, 0);
            }

            for (int j = 0;
                 j < PROCESS_COUNT;
                 ++j) {
                sem_close(semaphores[j]);
                sem_unlink(sem_names[j]);
            }

            return 1;
        }

        if (children[i] == 0) {
            int next = (i + 1) % PROCESS_COUNT;

            run_process(
                'A' + i,
                semaphores[i],
                semaphores[next],
                semaphores
            );
        }
    }

    /*
     * 父进程等待三个子进程结束。
     */
    for (int i = 0; i < PROCESS_COUNT; ++i) {
        int status;

        if (waitpid(children[i], &status, 0) == -1) {
            perror("waitpid");
            continue;
        }

        if (WIFEXITED(status)) {
            printf(
                "父进程：子进程%d退出，退出码=%d\n",
                children[i],
                WEXITSTATUS(status)
            );
        } else if (WIFSIGNALED(status)) {
            printf(
                "父进程：子进程%d被信号%d终止\n",
                children[i],
                WTERMSIG(status)
            );
        }
    }

    /*
     * 关闭父进程持有的信号量引用并删除名字。
     */
    for (int i = 0; i < PROCESS_COUNT; ++i) {
        if (sem_close(semaphores[i]) == -1) {
            perror("sem_close");
        }

        if (sem_unlink(sem_names[i]) == -1) {
            perror("sem_unlink");
        }
    }

    printf("\n所有子进程结束，信号量已经删除\n");
    return 0;
}