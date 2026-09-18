#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <string.h>

#define MQ_NAME "/posix_mq_demo"
#define MSG_SIZE 256

typedef struct {
    const char *text;
    unsigned int priority;
} Message;

int main(void)
{
    /*
     * 删除上一次异常退出可能留下的队列。
     */
    if (mq_unlink(MQ_NAME) == -1 &&
        errno != ENOENT) {
        perror("mq_unlink");
        return 1;
    }

    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = MSG_SIZE,
        .mq_curmsgs = 0
    };

    
    mqd_t queue = mq_open(
        MQ_NAME,
        O_CREAT | O_EXCL | O_WRONLY,
        0666,
        &attr
    );

    if (queue == (mqd_t)-1) {
        perror("mq_open");
        return 1;
    }

    Message messages[] = {
        {"低优先级消息", 1},
        {"高优先级消息", 10},
        {"普通优先级消息", 5}
    };

    int message_count =
        sizeof(messages) / sizeof(messages[0]);

    for (int i = 0; i < message_count; ++i) {
        size_t length =
            strlen(messages[i].text) + 1;

        if (mq_send(
                queue,
                messages[i].text,
                length,
                messages[i].priority
            ) == -1) {
            perror("mq_send");
            mq_close(queue);
            mq_unlink(MQ_NAME);
            return 1;
        }

        printf(
            "发送：%s，优先级=%u\n",
            messages[i].text,
            messages[i].priority
        );
    }

    if (mq_close(queue) == -1) {
        perror("mq_close");
        return 1;
    }

    /*
     * 这里不调用mq_unlink()。
     * 队列需要继续存在，让接收端稍后打开。
     */
    printf("消息发送完成，请运行接收端\n");
    return 0;
}