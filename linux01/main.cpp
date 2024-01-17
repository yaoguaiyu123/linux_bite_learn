//Date:2024/1/17
//Author:妖怪鱼
//Introduction:进程初学
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

//主版:连接各种硬件组件,实现硬件组件之间的数据传输
//设备之间的数据流动:本质是设备之间会进行数据的来回拷贝

//一个程序在运行的时候，必须先把程序先加载到内存

//操作系统是一个进行软硬件管理的软件
//操作系统会暴露一些接口，供上层开发使用，这部分由操作系统暴露的接口，就叫做系统调用

//进程信息被存放在一个叫做进程控制块的数据结构中,可以理解为进程信息的集合，一般称为PCB
//linux下PCB为task_struct
//task_struct是linux内核中的一种数据结构，他会被装载到RAM(内存)中并包含着进程的信息

//类似shell脚本，c语言拥有对系统调用的直接支持，也就是说，c语言可以直接调用底层的系统调用

int main()
{
    //    while (1) {
    //        sleep(1);
    //        printf("hello world\n");
    //    }

    printf("进程标识符pid: %d\n", getpid());
    printf("父进程标识符ppid: %d\n", getppid());
    return 0;
}
