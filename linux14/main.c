// Date:2026/9/17
// Author:妖怪鱼
// Introduction:进程间通信
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <mqueue.h>

// 匿名管道 pipe()
// 本质是内核中的一段缓冲区,使用read()和write()读写,数据是字节流，没有消息边界
// 匿名管道通常用于父子进程
// 管道为空时，read()默认阻塞,所有写端关闭后，read()返回 0，表示 EOF
void test01(){
    int pipefd[2];

    // 创建管道
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    printf("读端 fd = %d\n", pipefd[0]);
    printf("写端 fd = %d\n", pipefd[1]);

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }

    if (pid == 0) {
        /* 子进程只读取，所以关闭写端 */
        close(pipefd[1]);
        char buffer[128];
        printf("子进程：等待父进程发送数据...\n");
        fflush(stdout);
        
        // read只有管道为空，并且所有写端都关闭才返回，否则一直堵塞在这边
        ssize_t count = read(
            pipefd[0],
            buffer,
            sizeof(buffer) - 1
        );

        if (count == -1) {
            perror("read");
            close(pipefd[0]);
            return 1;
        }

        if (count == 0) {
            printf("子进程：管道写端全部关闭，读取到EOF\n");
        } else {
            buffer[count] = '\0';
            printf("子进程收到：%s\n", buffer);
        }

        close(pipefd[0]);
        return 0;
    }

    /* 父进程只写入，所以关闭读端 */
    close(pipefd[0]);

    sleep(2);

    const char *message = "hello from parent";

    ssize_t count = write(
        pipefd[1],
        message,
        strlen(message)
    );

    if (count == -1) {
        perror("write");
    } else {
        printf("父进程：写入了 %zd 字节\n", count);
    }
    // 关闭写端，关闭后，子进程继续读取时才能收到EOF。
    close(pipefd[1]);

    waitpid(pid, NULL, 0);
    printf("父进程：子进程已经结束\n");
    return 0;
}

// 命名管道 fifo
// int mkfifo(const char *pathname, mode_t mode);
// 会创建一个“管道文件”，存在于文件系统中，但是数据不会被写到磁盘，依旧在内核的管道缓冲区中
void test02(){
    // 示例代码见./fifo_reader.c 和 ./fifo_writer.c


}

// 共享内存
// 正常情况下，每个进程都有独立的虚拟地址空间
// 共享内存则把同一组物理内存页映射到多个进程的虚拟地址空间
// shm_open() 打开或创建共享内存
void test03(){
    // 示例代码见 ./shm_writer.c 和 ./shm_reader.c

}

// 信号量: 相当于一个由系统维护的非负整数计数器
// 核心操作
// sem_wait(); 申请资源，计数值减1
// sem_post(); 释放资源，计数值加1
void test04(){
    // 示例代码见 ./sem_waiter.c 和 ./sem_poster.c
    // 强化Demo: ./sem_order.c
}

// 消息队列
// 管道是连续字节流，而消息队列会保留每条消息的边界，并且支持消息优先级
void test05(){

    // 示例代码见 ./

}

int main(){


    return 0;
}