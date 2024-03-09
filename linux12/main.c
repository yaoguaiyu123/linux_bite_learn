// Date:2024/1/25
// Author:妖怪鱼
// Introduction:文件系统初学
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//每一个宏，对应的数值只有一个比特位，彼此位置不重叠
#define ONE 0x1
#define TWO 0x2
#define THREE 0x4
#define FOUR 0x8

// 文件 = 内容 + 属性
void test01()
{
    // c语言接口以写方式打开文件的时候，文件不存在就会创建文件
    //    FILE* file = fopen("../linux12/log.txt", "w");  // 以w的方式打开会清空文件
    FILE* file = fopen("../linux12/log.txt", "a"); // 以a的方式打开追加文件内容,不会清空文件
    if (file == NULL) {
        perror("fopen");
        return 1;
    }
    int cnt = 6;
    while (cnt--) {
        printf("%d\n", cnt);
        fprintf(file, "%s:%d\n", "你好世界", cnt);
    }
    fclose(file);
}

void test02()
{
    FILE* file = fopen("../linux12/log.txt", "r");
    if (file == NULL) {
        perror("fopen");
        return 1;
    }
    char buffer[255];
    int cnt = 6;
    // fgets将以'行'为单位读取文件中的内容，并且会保留内容末尾的\n
    while (cnt--) {
        fgets(buffer, sizeof(buffer), file);
        printf("%s", buffer);
    }
    fclose(file);
}

void show(int flags)
{
    if (flags & ONE) {
        printf("one\n");
    }
    if (flags & TWO) {
        printf("two\n");
    }
    if (flags & THREE) {
        printf("three\n");
    }
    if (flags & FOUR) {
        printf("four\n");
    }
}

// linux系统调用接口打开文件
void test03()
{
    umask(0);
    // 只写的方式打开文件,文件不存在就创建,0666表示文件的默认权限
    int fd = open("../linux12/test03.txt", O_WRONLY | O_CREAT, 0666);
//    int fd = open("../linux12/test01.txt", O_WRONLY | O_CREAT);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    close(fd);
}

// write写入文件
void test04()
{
    umask(0);
    // O_TRUNC会在打开文件的时候对文件内容作清空
    int fd = open("../linux12/test03.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        perror("open");
        return -1;
    }
    int cnt = 5;
    char buffer[255];
    while (cnt--) {
        sprintf(buffer, "%s---%d\n", "hello world", cnt);
        write(fd, buffer, strlen(buffer));
    }

    close(fd);
}

// read读取文件
void test05()
{
    umask(0);
    // O_TRUNC会在打开文件的时候对文件内容作清空
    // O_RDONLY表示以只读方式打开文件
    int fd = open("../linux12/test03.txt", O_RDONLY | O_CREAT, 0666);
    if (fd < 0) {
        perror("open");
        return -1;
    }
    char buffer[255];
    ssize_t num = read(fd, buffer, sizeof(buffer) - 1);
    printf("num = %d\n", num);
    if (num > 0) {
        buffer[num] = '\0';
        puts(buffer);
    }
    close(fd);
}

// 理解fd
//fd 是操作系统为了管理文件，为每个文件创建对应的内核数据结构标识
//每个进程都有一个文件描述符表，存放其打开的文件的fd
//fd = 0,1,2的文件是操作系统在进程启动的时候自动打开 de1
void test06()
{
    printf("stdin->fileno:%d\n", stdin->_fileno);
    printf("stdout->fileno:%d\n", stdout->_fileno);
    printf("stderr->fileno:%d\n", stderr->_fileno);
    char* p = "hello world 你好世界\n";
    write(1, p, strlen(p));
}

int main()
{
    //    test01();
    //    test02();
    //    show(ONE | THREE);
    //    test03();
    //    test04();
    //    test05();
    test06();
    return 0;
}










