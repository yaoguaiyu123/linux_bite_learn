// Date:2024/1/20
// Author:妖怪鱼
// Introduction:进程地址空间
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

// 栈区向低地址方向增长
// 堆区向高地址方向增长

//每一个进程都会有一个独立的地址空间，操作系统需要对每个地址空间做管理(先描述，再组织)
//将进程地址空间描述为mm_struct
//CR3 是一寄存器，用于存储页表的物理地址
//再通过页表来获取进程的地址空间的虚拟地址和物理地址的映射关系

//为什么需要进程地址空间?
//1.将物理内存由无序变得有序，让进程以统一的视角看待内存
//2.将进程管理和内存管理进行解耦
//3.地址空间和页表是保护内存安全的重要手段

//new/malloc申请内存本质是在进程的虚拟地址空间中申请

int g_unval;
int g_val = 100;

void test01()
{
    // c语言字符串数组的其实可以理解为一个2维数组，但是实际上内存的排布不是按二维数组的形式排布的
    char* strs[] = { "helloworld", "nihao" };
    printf("%p\n", strs);
    printf("%p\n", strs[0]);
    printf("%p\n", strs + 1);

    // 整型二维数组
    printf("\n整型二维数组的试验:\n");
    int arr[2][2] = { 1, 2, 3, 4 };
    printf("%p\n", arr);
    printf("%p\n", arr[0]); // 可以看到arr[0] 和 arr在数值上是同一个地址
    printf("%p\n", arr + 1);
}

void test02()
{
    //值不一样，但是地址一样，说明了&得到的不是物理地址,而是虚拟地址
    pid_t id = fork();
    if (id == 0) {
        // 子进程
        int cnt = 0;
        while (1) {
            printf("子进程的pid = %d ,ppid = %d ,g_val = %d ,&g_val = %p\n", getpid(), getppid(), g_val, &g_val);
            sleep(2);
            cnt++;
            if (cnt == 5) {
                g_val = 200;
            }
        }
    }else{
        while (1) {
            printf("父进程的pid = %d ,ppid = %d ,g_val = %d ,&g_val = %p\n", getpid(), getppid(), g_val, &g_val);
            sleep(2);
        }
    }
}

int main(int argc ,char *argv[],char *env)
{
    //    printf("main函数的地址: %p  (低地址)\n", main);
    //    printf("已初始化全局变量的地址: %p\n", &g_val);
    //    printf("未初始化全局变量的地址: %p\n", &g_unval);
    //    char* heap1 = (char*)malloc(20);
    //    char* heap2 = (char*)malloc(20);
    //    char* heap3 = (char*)malloc(20);
    //    char* heap4 = (char*)malloc(20);
    //    printf("堆区的地址: %p\n", heap1);
    //    printf("堆区的地址: %p\n", heap2);
    //    printf("堆区的地址: %p\n", heap3);
    //    printf("堆区的地址: %p\n", heap4);
    //    printf("栈区的地址: %p\n", &heap1);
    //    printf("栈区的地址: %p\n", &heap2);
    //    printf("栈区的地址: %p\n", &heap3);
    //    printf("栈区的地址: %p\n", &heap4);
    //    printf("argc的地址: %p\n", &argc);
    //    for (int i = 0; argv[i]; i++) {
    //        printf("argv[%d]的地址: %p\n", i, argv + i);
    //        for (int j = 0; argv[i][j] != '\0'; j++) {
    //            printf("argv[%d][%d]的地址: %p\n", i, j, argv[i] + j);
    //        }
    //    }
    //    for (int i = 0; env[i]; i++) {
    //        printf("env[%d]的地址: %p\n", i, env + i);
    //    }

    //    static int a;
    //    a = 1;
    //    printf("静态变量的地址: %p\n", &a);
    //    test02();
    return 0;
}
