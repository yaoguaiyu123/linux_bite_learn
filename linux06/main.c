// Date:2024/1/19
// Author:妖怪鱼
// Introduction:进程优先级,linux的调度和切换
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

// 进程优先级
// 前提:进程要访问某种资源，进程通过一定方式(排队),确认享受资源的先后顺序
// linux默认的优先级为80
// linux优先级是可以修改的，linux的优先级的范围是[60,99]
// 允许修改优先级但是修改的不是pri,而是nice值,pri(new) = pri(old) + nice
// 数字越小，优先级越高

//linux的调度和切换
//竞争性：cpu资源小于众多进程所需的资源
//独立性:多进程运行，需要独享资源，多进程运行期间互不干扰
//并行:多个进程在多个CPU下分别，同时运行
//并发:多个进程在一个CPU下采用进程切换的方式，在一段时间之内，让多个进程都得以推进

//调度:
//linux的runqueue中有一个活跃队列和一个过期队列，活跃队列中存放的是当前在准备被cpu调度的进程,
//过期队列中存放的是新加入但是未在排队被调度的进程，活动队列和国企队列中的进程按照优先级进行排序，
//其每个元素又是一个队列，存放的是优先级相同的进程，当活跃队列的进程调度完毕，就会直接交换二者的首地址的指针，
//让原来的活跃队列成为过期队列，让原来的过期队列成为活跃队列

//切换:
//当调度器决定切换到另一个进程时，就会进行上下文切换。
//上下文切换是指将当前运行进程的上下文（寄存器状态、内存映射等）保存起来，
//然后加载新进程的上下文，使其能够在CPU上执行。

void test01()
{
    printf("pid = %d\n", getpid());

}

int main()
{
    test01();
    return 0;
}
