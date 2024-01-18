//Date:2024/1/18
//Author:妖怪鱼
//Introduction:进程初学
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

// ps命令查看进程信息
// 可以在/proc目录下进程信息
// 所谓的当前路径，就是当前程序的pwd

//chdir 系统调用的接口，改变当前进程的工作目录   cwd得到改变,exe不变

int main()
{
    printf("pid:%d\n", getpid());
    chdir("/root");
    //    FILE* fp = fopen("../linux02/test.txt", "w"); // 以写入模式打开文件，如果文件不存在就创建文件
    FILE* fp = fopen("./hds.txt", "w");
    if (fp == NULL) {
        printf("文件打开失败\n");
        return -1;
    }
    fclose(fp);
    sleep(100);
    printf("程序即将退出\n");



    return 0;
}
