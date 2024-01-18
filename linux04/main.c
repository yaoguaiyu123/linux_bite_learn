// Date:2024/1/18
// Author:妖怪鱼
// Introduction:demo:一次创建多个进程
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

void Worker()
{
    //    int cnt = 12;
    //    while (cnt) {
    //        printf("子进程 pid = %d 正在工作中... cnt = %d\n", getpid(), cnt);
    //        cnt--;
    //        sleep(1);
    //    }

    printf("子进程 pid = %d 正在工作中...\n", getpid());
}

int main()
{
    int num = 10;
    for (int i = 0; i < num; ++i) {
        pid_t id = fork();
        if (id < 0) {
            return -1;
        }
        if (id == 0) {
            // 子进程
            Worker();
            exit(0); // 让子进程直接退出
        }
        printf("父进程完成一次循环...子进程的pid = %d\n", id);
        sleep(1); // 走到这里的只有父进程
    }
    sleep(15);
    return 0;
}
