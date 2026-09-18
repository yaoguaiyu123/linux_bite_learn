// Date:2026/9/17
// Author:妖怪鱼
// Introduction:多线程
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
#include <pthread.h>

// 线程
// 属于同一个进程
// 共享内存
// 共享全局变量
// 共享堆空间


void* thread_func01(void* arg)
{
    printf("hello thread\n");

    return NULL;
}

// 线程初识
void test01(){
    // 作为传入参数保存线程的 ID
    pthread_t tid;

    //创建线程
    int ret = pthread_create(
        &tid,           //线程ID
        NULL,           //线程属性
        thread_func01,    //线程执行函数
        NULL            //传递参数
    );


    if(ret != 0)
    {
        printf("pthread_create failed\n");
        return;
    }


    // 启动子线程，主线程阻塞，等待子线程结束
    pthread_join(
        tid,
        NULL
    );

    printf("hello main\n");
}

void* thread_func02(void* arg)
{

    // 将void*转换回int*
    int* num = (int*)arg;
    printf("thread num = %d\n", *num);
    return NULL;
}

// 线程参数传递
void test02(){
    pthread_t tid;

    int value = 100;
    pthread_create(
        &tid,
        NULL,
        thread_func02,
        &value
    );

    pthread_join(
        tid,
        NULL
    );

    printf("main end\n");
}

//共享变量
int count = 0;

void* thread_func03(void* arg)
{
    for(int i = 0; i < 1000000; i++)
    {
        count++;
    }
    return NULL;
}

// 演示线程不安全的情形
void test03(){
    pthread_t tid1;
    pthread_t tid2;
    //创建两个线程

    pthread_create(
        &tid1,
        NULL,
        thread_func03,
        NULL
    );

    pthread_create(
        &tid2,
        NULL,
        thread_func03,
        NULL
    );

    //等待线程结束
    pthread_join(
        tid1,
        NULL
    );

    pthread_join(
        tid2,
        NULL
    );

    // 最后输出的值不一定是2000000，因为count++其实分为三步：1. cpu读取count的值 2. count++ 3. 写回count的值
    printf(
        "count = %d\n",
        count
    );
}

//定义互斥锁
pthread_mutex_t mutex;

void* thread_func04(void* arg)
{
    for(int i = 0; i < 1000000; i++)
    {
        //加锁
        pthread_mutex_lock(&mutex);
        count++;
        //解锁
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

// 加锁
void test04(){
    pthread_t tid1;
    pthread_t tid2;

    //初始化mutex
    pthread_mutex_init(
        &mutex,
        NULL
    );

    pthread_create(
        &tid1,
        NULL,
        thread_func04,
        NULL
    );
    pthread_create(
        &tid2,
        NULL,
        thread_func04,
        NULL
    );

    pthread_join(
        tid1,
        NULL
    );
    pthread_join(
        tid2,
        NULL
    );

    printf(
        "count = %d\n",
        count
    );

    pthread_mutex_destroy(
        &mutex
    );
}

// 条件变量


int main(){
    // test01();
    // test02();
    // test03();
    test04();

    return 0;    
}