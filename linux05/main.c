// Date:2024/1/18
// Author:妖怪鱼
// Introduction:进程状态
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
// 1.进程排队
// task_struct中有一个listNode的双向链表节点的成员,通过这些节点形成一个双向链表来维护一个队列
// 假设listNode类型的成员变量名为n, 则可以通过 &n - &((task_struct*)0 -> n) 来得到该task_struct对象的地址

// 2.进程状态
// 所谓的状态，在task_struct中表现为一个整型变量
// 运行,阻塞,挂起

// 运行状态:task_struct连入运行队列

// 阻塞状态
// 当我们的进程在进行等待软硬件资源的时候，资源如果没有就绪，我们的进程task_struct只能
// 1.将自己设置为阻塞状态 2.将自己的PCB连入等待的资源提供的等待队列
// 课程 进程状态 3:00~3:30

// 挂起状态
// 前提:计算机资源已经比较吃紧了
// 挂起:在资源比较吃紧的时候，操作系统会将内存中不重要的资源换入到磁盘的swap分区

// 3.linux下具体的进程状态
// R(running)，S(sleep)，D(disk sleep),T(stopped),T(tracing stop),Z(zombie),X(dead)
// S(sleep)，D(disk sleep),T(stopped),T(tracing stop)  都可以归类到阻塞状态

void test01()
{
    // 当以下代码运行的时候，进程的STAT为S+
    // 因为大部分的时间是在sleep
    while (1) {
        printf("hello process!pid = %d\n", getpid());
        sleep(1);
    }
}

void test02()
{
    // 当以下代码运行的时候，进程的STAT为S+
    // 因为大部分时间也是在等待，这部分的等待是cpu等待数据刷新显示在外设上，cpu的数据远远大于外设
    // 所以需要进行等待
    while (1) {
        printf("hello process!pid = %d\n", getpid());
    }
}

void test03()
{
    // 当以下代码运行的时候，进程的STAT为R+
    // 此时进程处于运行状态
    printf("hello process!pid = %d\n", getpid());
    int i = 1;
    while (1) {
        i++;
    }
}

void test04()
{
    // 当以下代码运行的时候，进程的STAT为S+
    // 当前进程在等待外设资源，所以进程处于阻塞状态,表现为S状态(可中断睡眠，浅度睡眠)
    printf("hello process!pid = %d\n", getpid());
    int a;
    scanf("%d", &a);
    printf("a = %d\n", a);
}

void test05()
{
    // 当以下代码调试到断点的时候，进程的STAT为t+(tracing stop)
    printf("hello process!pid = %d\n", getpid());
    int a = 1;
    int b = 1;
    printf("%d %d\n", a, b);
}

void test06()
{
    // 僵尸进程:当子进程已经退出，但是父进程没有通过wait或者waitpid来获取子进程的资源，那么子进程就会成为僵尸进程
    pid_t id = fork();
    if (id == 0) {
        // 子进程
        int cnt = 5;
        while (cnt--) {
            printf("我是子进程,pid = %d,ppid = %d\n", getpid(), getppid());
            sleep(1);
        }
        exit(1); // 子进程直接退出
    }
    int cnt = 10;
    while (cnt--) {
        // 父进程
        printf("我是父进程,pid = %d,ppid = %d\n", getpid(), getppid());
        sleep(1);
    }

    wait(); // 父进程回收子进程,获取子进程的退出信息，子进程不再是僵尸进程
    // 当父进程调用wait函数的时候，父进程会阻塞，直到有子进程退出
    // 当有子进程退出，wait函数就会返回该子进程的pid
}

void test07()
{
    //当父进程退出，子进程还在运行的时候，子进程就会成为孤儿进程，此时子进程的新的父进程将会是init进程
    pid_t id = fork();
    if (id == 0) {
        printf("我是子进程,pid = %d,ppid = %d\n", getpid(), getppid());
        sleep(10);
        exit(1);
    }
    printf("我是父进程,pid = %d,ppid = %d\n", getpid(), getppid());
}

void test08()
{
    // wait函数进阶使用
    // 如果父进程没有任何子进程，wait的返回值
    pid_t i = wait();
    printf("i = %d\n", i); //-1
}

void test09()
{
    // wait函数进阶使用
    // 多个子进程的使用wait进行等待回收
    pid_t id1 = fork();
    if (id1 == 0) {
        printf("我是子进程,pid = %d,ppid = %d\n", getpid(), getppid());
        sleep(1);
        exit(1);
    }
    pid_t id2 = fork();
    if (id2 == 0) {
        printf("我是子进程,pid = %d,ppid = %d\n", getpid(), getppid());
        sleep(2);
        exit(1);
    }
    pid_t id3 = fork();
    if (id3 == 0) {
        printf("我是子进程,pid = %d,ppid = %d\n", getpid(), getppid());
        sleep(3);
        exit(1);
    }
    int res;
    while ((res = wait()) != -1) {
        printf("子进程 %d 得到回收\n", res);
    }
    printf("所有子进程已经回收完毕\n");


}

int main()
{
    //    test01();
    //    test02();
    //    test03();
    //    test04();
    //    test05();
    //    test06();
    //    test07();
    //    test08();
    test09();
    return 0;
}





