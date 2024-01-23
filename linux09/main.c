// Date:2024/1/22
// Author:妖怪鱼
// Introduction:进程控制1(进程退出，进程等待)
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// 写时拷贝
// 当使用fork创建一个进程的时候，操作系统会根据父进程的页表构建子进程的页表，但是
// 父进程页表中rw权限的条目到子进程页表中的权限将会是r(只读)
// 当子进程尝试写入的发现权限不够，此时触发缺页中断,然后为子进程的数据申请新的空间，
// 将子进程的页表中的相关条目的权限修改为rw,这就是写时拷贝的权限

// main函数的返回值---进程的退出码,一般为0表示成功，非0表示失败

// 进程退出的时候，操作系统做了什么
// 释放地址空间，释放页表，释放PCB

// 进程等待
// 当一个子进程退出的时候，如果父进程不进行等待回收，子进程就会成为僵尸进程
// 父进程通过wait方式回收子进程的资源，获取子进程的退出信息

void test01()
{
    char* str = "Hello World";
    *str = "H";
    // 上述代码运行时会崩溃
    // str 指向的是一个字符串字面量，而字符串字面量是不可修改的
    // 本质是页表中的条目是只读的
}

// 进程退出错误码
void test02()
{
    // 将一个错误码转换为具体错误描述
    for (int i = 0; i < 200; ++i) {
        printf("%s\n", strerror(i));
    }
}

// 函数退出错误码
void test03()
{
    // errno 是一个用于表示函数调用是否出错的全局变量
    // 当函数调用失败的时候，就会修改这个全局变量的值
    FILE* file = fopen("nihao.txt", "r");

    if (file == NULL) {
        perror("打开文件失败");
        printf("errno = %d\n", errno);
    }
}

void test04()
{
    printf("nihao shijie!");

    exit(3); // 终止进程,参数是进程退出的时候的退出码
    //    _exit(3);    //_exit与exit函数做用法一样，但是_exit函数是系统调用,exit函数是c语言函数

    // 具体的区别的话就是exit在退出的时候会支持刷新缓冲区
    //_exit不支持刷新缓冲区
}

// wait函数
// 默认进行阻塞等待
// 等待任意一个子进程
//
void test05()
{
    pid_t id = fork();
    if (id == 0) {
        int cnt = 5;
        while (cnt) {
            printf("我是子进程,pid = %d,ppid = %d\n", getpid(), getppid());
            sleep(1);
            cnt--;
        }
        exit(0);
    }

    printf("我是父进程,pid = %d,ppid = %d\n", getpid(), getppid());
    pid_t rid = wait(NULL); // 父进程阻塞等待
    if (rid > 0) {
        printf("子进程%d退出,并得到父进程的等待\n", rid);
    }
}

// waitpid函数
//进程退出码是一个整数值，表示进程正常终止时的状态。
//进程退出时收到的信号编号是在进程非正常终止时产生的。
void test06()
{
    pid_t id = fork();
    if (id == 0) {
        int cnt = 5;
        while (cnt) {
            printf("我是子进程,pid = %d,ppid = %d\n", getpid(), getppid());
            sleep(1);
            cnt--;
        }
        exit(1);
    }

    printf("我是父进程,pid = %d,ppid = %d\n", getpid(), getppid());
    int status = 0;
    //status用于存储子进程的终止状态信息,分为退出码，core dump标志，退出信号等等多个部分
    pid_t rid = waitpid(id, &status,0); // 父进程阻塞等待指定的子进程,这里的0参数表示阻塞等待
    if (rid > 0) {
        printf("子进程%d退出,并得到父进程的等待,子进程的退出状态为:%d\n", rid, status);   //status = 256
    }

}


int main()
{
    //    test01();
    //    test02();
    //    test03();
    //    test04();
    //    test05();
    test06();

    return 0;
}








