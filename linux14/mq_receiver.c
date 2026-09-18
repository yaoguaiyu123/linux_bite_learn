#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>

#define MQ_NAME "/posix_mq_demo"
#define MSG_SIZE 256
#define MESSAGE_COUNT 3

int main(void)
{
    mqd_t queue = mq_open(
        MQ_NAME,
        O_RDONLY
    );

    if (queue == (mqd_t)-1) {
        perror("mq_open");
        return 1;
    }

    printf("成功打开消息队列\n");

    for (int i = 0; i < MESSAGE_COUNT; ++i) {
        char buffer[MSG_SIZE + 1];
        unsigned int priority;

        ssize_t count = mq_receive(
            queue,
            buffer,
            MSG_SIZE,
            &priority
        );

        if (count == -1) {
            perror("mq_receive");
            mq_close(queue);
            return 1;
        }

        /*
         * 确保字符串以'\0'结束。
         */
        buffer[count] = '\0';

        printf(
            "接收：%s，字节数=%zd，优先级=%u\n",
            buffer,
            count,
            priority
        );
    }

    if (mq_close(queue) == -1) {
        perror("mq_close");
        return 1;
    }

    if (mq_unlink(MQ_NAME) == -1) {
        perror("mq_unlink");
        return 1;
    }

    printf("消息队列已经删除\n");
    return 0;
}