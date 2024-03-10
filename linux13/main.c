// Date:2024/3/9
// Author:妖怪鱼
// Introduction:文件fd
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

// 文件描述符的分配规则：在files_struct数组当中，找到当前没有被使用的
// 最小的一个下标，作为新的文件描述符。
// 重定向的本质:文件描述符对应的file*发生改变
void test01()
{
    int fd = open("myfile01.txt", O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    printf("fd = %d\n", fd);
    close(1); // 关掉标准输出
    int fd_1 = open("myfile02.txt", O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (fd_1 < 0) {
        perror("open");
        return 1;
    }
    printf("fd_1 = %d\n", fd_1);
    printf("hello world"); // 输出重定向
    close(1);
    close(fd);
}

// 使用dup2系统调用
// int dup2(int oldfd, int newfd);
// 将oldfd中的文件描述符拷贝到newfd,本质上是file*指针的拷贝。如果newfd已经有打开的文件，就会先将其关闭
void test02()
{
    int fd = open("myfile01.txt", O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    printf("fd = %d\n", fd);
    dup2(fd, 1);
    printf("你好世界确实是不错的");
    write(fd, "这是新添加的前缀   ", 27);
    close(fd);
    printf("   这是后缀");
    // 只有当所有指向该文件表项的文件描述符都被关闭时，内核才会释放文件表项，关闭文件。
    // 采用类似于shared_ptr的引用技术
}

// 输入重定向
void test03()
{
    int fd = open("myfile01.txt", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    dup2(fd, 0);
    char arr[255];
    // 本应该是从标准输入缓冲区中读取数据，重定向之后是从myfile01文件中读取数据
    while (fgets(arr, sizeof(arr), stdin)) {
        printf("%s\n", arr);
    }
}

// 对于每一个文件，操作系统都创建了一个struct file
void test04(){
    const char *msg0="hello printf\n";
    const char *msg1="hello fwrite\n";
    const char *msg2="hello write\n";
    printf("%s", msg0);
    fputs("hello fputs\n", stdout);
    fwrite(msg1, strlen(msg0), 1, stdout);
    write(1, msg2, strlen(msg2));
    fork();
}




int main()
{
    //    test01();
    //    test02();
    //    test03();
    //    test04();
    return 0;
}









