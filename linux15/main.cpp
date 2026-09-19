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
#include <queue>
#include <semaphore.h>


using namespace std;

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

    // tip 创建线程， 线程从这里开始运行
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


    // 
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

// note 条件变量和锁
// 条件变量:控制线程什么时候可以继续执行
// 锁：控制线程什么时候可以访问资源
// 两者一般配合使用，因为要保证生产、消费的时候也是加锁的

//共享队列
queue<int> task_queue;

//条件变量
pthread_cond_t cond;

// 生产者
void* producer(void* arg)
{
    for(int i = 1; i <= 5; i++)
    {

        //加锁
        pthread_mutex_lock(&mutex);
        //生产任务
        task_queue.push(i);
        printf(
            "producer produce: %d\n",
            i
        );

        //通知消费者
        pthread_cond_signal(&cond);

        //解锁
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return NULL;
}

// 消费者
void* consumer(void* arg)
{
    while(true)
    {
        pthread_mutex_lock(&mutex);

        // tip 为什么是 while 而不是 if，因为可能有多个消费者，而 pthread_cond_signal 可能会同时唤醒多个
        while(task_queue.empty())
        {
            // tip pthread_cond_wait内部机制
            // 1.把当前线程加入一个等待队列 等待队列就是一个链表，记录"有哪些线程在等这个条件变量"
            // 将来有生产者调用 pthread_cond_signal 的时候，就知道该叫醒哪些线程
            // 2. 释放锁然后挂起(原子操作)，挂起就是让操作系统把这个线程暂停，不再分配 CPU 时间给它，去跑别的线程
            // 3. 等待条件变量被叫醒，然后继续持有锁
            pthread_cond_wait(
                &cond,
                &mutex  // 这里要传入锁
            );
        }

        //取任务
        int task = task_queue.front();
        task_queue.pop();
        printf(
            "consumer consume: %d\n",
            task
        );
        pthread_mutex_unlock(&mutex);

    }
    return NULL;
}

void test05(){
    pthread_t producer_tid;
    pthread_t consumer_tid;

    //初始化mutex
    pthread_mutex_init(
        &mutex,
        NULL
    );

    //初始化条件变量
    pthread_cond_init(
        &cond,
        NULL
    );

    pthread_create(
        &producer_tid,
        NULL,
        producer,
        NULL
    );

    pthread_create(
        &consumer_tid,
        NULL,
        consumer,
        NULL
    );

    pthread_join(
        producer_tid,
        NULL
    );

    pthread_join(
        consumer_tid,
        NULL
    );

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

// note 信号量、锁
// 信号量：管理某种资源数量 本质是一个计数器
// 锁：本质上就是一个计数只能是 0 或 1 的信号量
// 区别：所有权，Mutex只有"加锁的线程"才能"解锁"，Semaphore任何线程都可以
// Semaphore限制很少，在一个线程里不进行sem_wait而进行多次sem_post，资源量可以超过初始值

// tip 一个资源池里面有 3 个资源，但是有 5 个线程竞争使用
sem_t sem;

void* worker(void* arg)
{
    // 获取线程id
    int id = *(int*)arg;
    printf("thread %d waiting resource\n", id);

    //申请资源
    sem_wait(&sem);
    printf("thread %d get resource\n", id);

    //模拟使用资源
    sleep(2);
    printf("thread %d release resource\n", id);

    //释放资源
    sem_post(&sem);

    return NULL;
}

void test06()
{
    pthread_t tids[5];
    int ids[5];

    //初始化信号量
    //资源数量为3
    sem_init(&sem, NULL, 3);
    for(int i = 0; i < 5; i++)
    {
        ids[i] = i + 1;
        pthread_create(
            &tids[i],
            NULL,
            worker,
            &ids[i]
        );
    }

    for(int i = 0; i < 5; i++)
    {
        pthread_join(
            tids[i],
            NULL
        );
    }

    sem_destroy(&sem);
}

// note 线程回收
// 在子线程的线程函数里，pthread_exit(value)和return value是等价的
// 但在主线程里面，return会导致整个进程结束，所有线程退出，而pthread_exit(value)只是结束当前线程，其他线程继续执行
void* thread_func07(void* arg)
{
    printf("thread running\n");

    int* value = new int(100);

    pthread_exit(value);   // 结束当前线程，并返回一个value

    return NULL;
}

void test07()
{
    pthread_t tid;

    pthread_create(
        &tid,
        NULL,
        thread_func07,
        NULL
    );

    void* ret;  // 定义一个传入参数获取线程的返回值
    pthread_join(
        tid,
        &ret
    );

    int* value = (int*)ret;

    printf("thread return value = %d\n", *value);

    delete value;
}

// note 线程detach
// pthread_detach

void* worker08(void* arg)
{
    printf("worker start\n");

    sleep(3);

    printf("worker finish\n");

    return NULL;
}

void test08()
{
    pthread_t tid;

    pthread_create(
        &tid,
        NULL,
        worker08,
        NULL
    );
    
    // 分离线程，主线程不等待子线程结束, 子线程资源由系统自动回收而不是使用join回收，主线程可能先于子线程退出
    pthread_detach(tid);  
    printf("main finish\n");
    sleep(5);
}


int main(){
    // test01();
    // test02();
    // test03();
    // test04();
    test05();

    return 0;    
}