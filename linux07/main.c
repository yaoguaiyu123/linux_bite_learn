// Date:2024/1/20
// Author:妖怪鱼
// Introduction:环境变量
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

// 命令行参数
// argc argv

// 环境变量
// 系统的环境变量，本质就是系统开辟的空间
// 环境变量具有全局属性，环境变量会被所有的子进程进行继承
// export 导入环境变量

//bash_profile 是在 Bash shell 用户登录时执行的脚本文件，通常用于配置用户的个性化环境
//bashrc 文件中的内容在每次打开新的 Bash shell 时都会执行，而不是在用户登录时


void test01()
{
    // 获取环境变量
    const char* username = getenv("USERNAME"); // getenv获取环境变量的函数
    printf("username: %s\n", username);
}

void test02()
{
    // 获取环境变量
    const char* username = getenv("USERNAME");
    if (username == NULL) {
        printf("登录失败\n");
        exit(1);
    }
    if (strcmp(username, "吴博渊") == 0) {
        printf("欢迎USER %s 的登录\n", username);
    } else {
        printf("你不是高贵的吴博渊,请你离开\n");
    }
}

void test03()
{
    // 输出环境变量的另一种方式
    //  environ

    extern char** environ; // 声明变量
    printf("打印环境变量\n");
    for (int i = 0; environ[i]; i++) {
        printf("environ[%d] = %s\n", i, environ[i]);
    }

}

int main(int argc, char* argv[], char* env[])
{
    // 命令行参数
//    printf("argc = %d\n", argc);
//    for (int i = 0; i < argc; ++i) {
//        printf("argv[%d] = %s\n", i, argv[i]);
//    }

//    if (argc != 2) {
//        printf("参数错误\n");
//        exit(1);
//    }
//    if (strcmp(argv[1], "-1") == 0) {
//        printf("这是第一种功能\n");
//    } else if (strcmp(argv[1], "-2") == 0) {
//        printf("这是第二种功能\n");
//    } else {
//        printf("参数错误\n");
//    }


    //环境变量
    //    printf("\n接下来打印环境变量!!!!\n\n");

    //    for (int i = 0; env[i]; i++) {
    //        printf("env[%d]: %s\n", i, env[i]);
    //    }

    //    pid_t id = fork();
    //    if (id == 0) {
    //        for (int i = 0; env[i]; i++) {
    //            printf("子进程的env[%d]: %s\n", i, env[i]);
    //        }
    //    }

    //    test01();
    //    test02();
    test03();
    return 0;
}




