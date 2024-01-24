// Date:2024/1/23
// Author:妖怪鱼
// Introduction:进程控制2(进程退出，进程程序替换)
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

// 进程退出码和他退出信号会被写入到task_struct中
// 读取子进程退出信息本质是读取内核数据


// int sub_exitcode = 0;7
// int sub_exitsigno = 0;

typedef void (*fun_t)(); // 定义一个函数指针类型
fun_t tasks[5]; // 创建一个fun_t类型的数组

void printLog()
{
    printf("打印日志\n");
}

void printNet()
{
    printf("打印网络\n");
}

void printNPC()
{
    printf("刷新NPC\n");
}

void initTask()
{
    printf("初始化任务\n");
    tasks[0] = printLog;
    tasks[1] = printNet;
    tasks[2] = printNPC;
    tasks[3] = NULL;
}

void excuteTask()
{
    for (int i = 0; tasks[i]; ++i) {
        tasks[i]();
    }
}

// 进程退出码和退出信号
void test01()
{
    pid_t id = fork();
    if (id == 0) {
        int cnt = 50;
        while (cnt) {
            printf("我是子进程,pid = %d,ppid = %d,cnt = %d\n", getpid(), getppid(), cnt);
            sleep(1);
            cnt--;
            if (cnt == 45) {
                int* p = NULL;
                *p = 100; // 这里发生运行时错误等待进程退出
            }
        }
        exit(3);
    }

    printf("我是父进程,pid = %d,ppid = %d\n", getpid(), getppid());
    int status = 0;
    pid_t rid = waitpid(id, &status, 0); // 父进程阻塞等待
    if (rid > 0) {
        printf("子进程%d退出,并得到父进程的等待,exit signo = %d,exit code = %d\n", rid, status & 0x7F, (status >> 8) & 0xFF);
        bool flag1 = WIFEXITED(status); // 若为正常终止的子进程返回的状态，则为真
        pid_t id_1 = WEXITSTATUS(status); // 若WIFEXITED非0,则提取子进程退出码
        printf("%d %d\n", flag1, id_1);
    }
}

// waitpid的非阻塞等待,当前没有子进程退出就返回0
void test02()
{
    pid_t id = fork();
    if (id == 0) {
        int cnt = 5;
        while (cnt) {
            printf("我是子进程,pid = %d,ppid = %d,cnt = %d\n", getpid(), getppid(), cnt);
            sleep(1);
            cnt--;
        }
        exit(3);
    }
    printf("我是父进程,pid = %d,ppid = %d\n", getpid(), getppid());
    int status = 0;
    initTask();
    // 使用非阻塞等待实现多次轮询访问
    while (true) {
        pid_t rid = waitpid(id, &status, WNOHANG); // 父进程非阻塞等待
        if (rid > 0) {
            printf("父进程等待到子进程%d退出,exit signo = %d,exit code = %d\n", rid, status & 0x7F, (status >> 8) & 0xFF);
            break;
        } else if (rid == 0) {
            printf("------------当前没有子进程退出----------------\n");
            printf("------------开始执行父进程的任务--------------\n");
            excuteTask();
            printf("------------结束执行父进程的任务--------------\n");
        } else {
            perror("waitpid");
            break;
        }
        sleep(1);
    }
}

// 进程程序替换
// 进程程序替换是指在一个运行中的进程内部，将当前正在执行的程序替换为另一个程序的过程。
// 原理是用新的程序的代码和数据替换老程序的代码和数据
// 这个过程并没有创建新的进程
// 使用execl系统调用完成上述功能
// exec*的函数没有成功返回，只有失败返回

// 创建一个进程，先创建PCB,地址空间，页表等等,再将程序加载到内存中
void test03()
{
    printf("我是一个进程pid = %d\n", getpid());
    printf("开始程序替换\n");
//    execl("../linux10/child","child",NULL);
    execl("/usr/bin/ls","ls" ,"-la", NULL);   //arg0一般是要执行的程序的名称
    printf("结束程序替换\n");
}

// 使用子进程进行程序替换
void test04()
{
    printf("我是一个父进程pid = %d\n", getpid());
    pid_t id = fork();
    if (id == 0) {
        sleep(1);
        printf("我是一个子进程pid = %d\n", getpid());
        printf("开始程序替换\n");
        execl("/usr/bin/ls", "ls", "-la", NULL);
        exit(1);
    }
    pid_t rid = waitpid(id, NULL, 0);   //阻塞等待
    printf("wait success!\n");
}

// execlp在执行时会在系统的路径变量（PATH 环境变量）中查找可执行文件。所以只需指定要执行的程序的名称，不需要提供完整的路径。
void test05()
{
    printf("我是一个进程pid = %d\n", getpid());
    printf("开始程序替换\n");
    execlp("ls", "ls", "-la", NULL);
    printf("结束程序替换\n");
}

// execv 使用一个以 NULL 结尾的字符串数组来传递参数
void test06()
{
    printf("我是一个进程pid = %d\n", getpid());
    printf("开始程序替换\n");
    char* args[] = { "ls", "-l", "-a", NULL }; // 创建一个字符串数组
    execv("/usr/bin/ls", args);
    printf("结束程序替换\n");
}

// execvp 使用一个以 NULL 结尾的字符串数组来传递参数,并且不需要对可执行文件提供完整路径
void test07()
{
    printf("我是一个进程pid = %d\n", getpid());
    printf("开始程序替换\n");
    char* args[] = { "ls", "-l", "-a", NULL }; // 创建一个字符串数组
    execvp("ls", args);
    printf("结束程序替换\n");
}

// 创建子进程bash
void test08()
{
    printf("我是一个父进程pid = %d\n", getpid());
    pid_t id = fork();
    if (id == 0) {
        sleep(1);
        printf("我是一个子进程pid = %d\n", getpid());
        printf("开始程序替换\n");
        execl("/usr/bin/bash", "bash", NULL);
        exit(1);
    }
    pid_t rid = waitpid(id, NULL, 0); // 阻塞等待
    if (rid > 0) {
        printf("wait success!\n");
    }
}

// 给子进程新增环境变量 putenv
void test09()
{
    printf("我是一个父进程pid = %d\n", getpid());
    putenv("VALUE=bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    pid_t id = fork();
    if (id == 0) {
        sleep(1);
        printf("我是一个子进程pid = %d\n", getpid());
        printf("开始程序替换\n");
        // 获取环境变量的值
        char* value = getenv("VALUE");
//        execl("/usr/bin/echo", "echo", value, NULL);
        execl("../linux10/test", "test", NULL);
        exit(1);
    }
    pid_t rid = waitpid(id, NULL, 0); // 阻塞等待
    if (rid > 0) {
        printf("wait success!\n");
    }
}

// execle,excve,execvpe等等多了一个'e',这些函数参数多了一个char* const evnp[]的参数
// 可以自定义环境变量表来给子进程重置环境变量
void test10()
{
    char* const env[] = { "WBY=wuboyuan", "PATH=/" }; // 自定义一个环境变量表
    printf("我是一个父进程pid = %d\n", getpid());
    pid_t id = fork();
    if (id == 0) {
        sleep(1);
        printf("我是一个子进程pid = %d\n", getpid());
        printf("开始程序替换\n");
        execle("../linux10/test", "test", NULL, env);
        exit(1);
    }
    pid_t rid = waitpid(id, NULL, 0); // 阻塞等待
    if (rid > 0) {
        printf("wait success!\n");
    }
}

//实际上程序替换的系统调用只有execve,其他都是对execve的封装


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
    //    test09();
    test10();
    return 0;
}
