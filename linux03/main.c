//Date:2024/1/18
//Author:妖怪鱼
//Introduction:手动创建进程fork函数
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

//通过fork系统调用创建进程
//fork函数的返回值(有两个返回值):父进程中返回子进程的pid,子进程中返回0
//1.fork函数为什么会返回两次(有两个返回值)?
//答:fork之前只有一个进程在运行，所以调用fork的一定是父进程,当执行fork函数执行到return的时候，实际上子进程已经被创建了,
//所以实际上这里是两个进程在分别执行fork函数中的的return语句

//2.同一个变量id为什么既等于0,又大于0(在父子进程中都打印id的地址，发现地址是一样的)
//答:任意一个进程之间是具有独立性的，互相不能影响,当父子进程对同一个变量进行修改的时候，会发生写时拷贝,
//这时候这个变量就会在不同的进程中有不同的进程地址空间。也就是说，这个id变量在不同进程中的虚拟地址是相同的,
//表现为这个变量的&的值一样，但是当映射到物理地址的时候，物理地址是不同的

//父子进程进行代码共享，各自开辟空间

int main()
{
    printf("before fork , pid = %d , ppid = %d\n", getpid(), getppid());
    pid_t id = fork();
    //    sleep(2);


    if (id == 0) {
        // 子进程要做的事情
        while (1) {
            printf("%p ", &id);
            printf("这里是子进程 ");
            printf("after fork , pid = %d , ppid = %d , returnId = %d\n", getpid(), getppid(), id);
            sleep(1);
        }
    } else {
        // 父进程要做的事情
        while (1) {
            printf("%p ", &id);
            printf("这里是父进程 ");
            printf("after fork , pid = %d , ppid = %d , returnId = %d\n", getpid(), getppid(), id);
            sleep(1);
        }
    }



    return 0;
}
