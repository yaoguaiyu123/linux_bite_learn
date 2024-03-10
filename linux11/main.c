// Date:2024/1/24
// Author:妖怪鱼
// Introduction:自定义shell
/// 重要的一点，普通空格在ascii表中是32(space),而不是\0对应的0
/// c语言字符串是以\0结尾，而不是以空格结尾
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <assert.h>
#include <fcntl.h>
// 定义 ANSI 转义码,改变printf的文字颜色
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define SIZE 1024

#define NONE_REDIR 0
#define INPUT_REDIR 1
#define OUTPUT_REDIR 2
#define APPEND_REDIR 3
int redirType = NONE_REDIR;
char* redirFile = NULL;

char pwd[SIZE];
char env[3][SIZE];
int env_cnt = 0;
int last_code = 0;
char special[SIZE / 32];

char* HostName()
{
    char* hostname = getenv("HOSTNAME");
    if (!hostname) {
        hostname = "mjr-pc";  //这边使用环境变量的方式或者直接系统调用都无法获取，但是命令行是可以获取的
    }
    return hostname;
}

char* UserName(){
    char* username = getenv("USER");
    if (!username) {
        username = "None";
    }
    return username;
}

char * Home(){
    char* home = getenv("HOME");
    if (!home) {
        home = "/root";
    }
    return home;
}

char* CurrentWorker(){
    char* currentworker = getenv("PWD");
    if (!currentworker) {
        currentworker = "None";
    }
    return currentworker;
}

void test01() {
    // 使用 ANSI 转义码设置文字颜色和背景颜色
    printf(ANSI_COLOR_RED "This is red text" ANSI_COLOR_RESET "\n");
    printf(ANSI_COLOR_GREEN "This is green text" ANSI_COLOR_RESET "\n");
    printf(ANSI_COLOR_YELLOW "This is yellow text" ANSI_COLOR_RESET "\n");
    printf(ANSI_COLOR_BLUE "This is blue text" ANSI_COLOR_RESET "\n");
    printf(ANSI_COLOR_MAGENTA "This is magenta text" ANSI_COLOR_RESET "\n");
    printf(ANSI_COLOR_CYAN "This is cyan text" ANSI_COLOR_RESET "\n");
}

// 面对输出重定向的情况进行字符串分割
void CommandCheck(char* line)
{
    assert(line);
    char* pos = line;
    //*pos一直到\0结束循环，如果遇到重定向符号就提前退出循环
    while (*pos) {
        if (*pos == '>') {
            if (*(pos + 1) == '>') {
                //>>表示追加到文件末尾
                *pos = '\0';
                *(pos + 1) = '\0';
                pos += 2;
                while (isspace(*pos)) {
                    pos++;
                }
                redirType = APPEND_REDIR;
                redirFile = pos;
                //                redirType = er
                break;

            } else {
                *pos = '\0';
                pos++;
                while (isspace(*pos)) {
                    pos++;
                }
                redirFile = pos;
                redirType = OUTPUT_REDIR;
                break;
            }
        } else if (*pos == '<') {
            *pos = '\0';
            pos++;
            while(isspace(*pos)) pos++;
            redirFile = pos;
            redirType=INPUT_REDIR;
            break;
        }
        ++pos;
    }
}

//交互函数
int Interactive(char out[],int size){
    printf(ANSI_COLOR_RED "[" ANSI_COLOR_CYAN "%s@%s" ANSI_COLOR_YELLOW " %s" ANSI_COLOR_RED
                          "]" ANSI_COLOR_CYAN "$ " ANSI_COLOR_RESET,
        UserName(), HostName(), CurrentWorker());
    fgets(out, size, stdin); // 获取整行字符串(包括\n)
    out[strlen(out) - 1] = '\0'; // 去掉字符串末尾的\n

    CommandCheck(out);

    return strlen(out);
}

// 切割字符串的函数
void Split(char* line, char* strs[])
{
    int i = 0;
    strs[i++] = strtok(line, " ");
    while (strs[i++] = strtok(NULL, " ")) {
        ;
    }
    // 如果是ls命令就加上--color参数
    if (strcmp(strs[0], "ls") == 0) {
        strcpy(special, "--color");
        strs[i - 1] = special;
        strs[i] = NULL;
    }
//    for (int j = 0; strs[j]; ++j) {
//        printf("%s\n", strs[j]);
//    }

}

int BuildingCmd(char* strs[])
{
    int ret = 0;
    // 判断是不是有重定向
    if (redirType != NONE_REDIR) {
        return ret;
    }
    // 处理内建命令
    if (strcmp(strs[0], "cd") == 0) {
        ret = 1;
        char* target = strs[1];
        if (!target) {
            target = Home();
        }
        chdir(target); // 调用系统调用改变工作目录
        // 使用snprintf和getcwd接口
        char tmp[SIZE];
        getcwd(tmp, SIZE);
        snprintf(pwd, SIZE, "PWD=%s", tmp);
        putenv(pwd); // 更新环境变量
    } else if (strcmp(strs[0], "export") == 0) {
        ret = 1;
        if (strs[1]) {
            if (env_cnt < 3) {
                strcpy(env[env_cnt], strs[1]);
                // 为什么需要拷贝?
                // 因为实际上strs[1]是一个char*指针，环境变量绑定的是这个指针，当再次输入改变commandLine之后,就会改变这个指针所指向的内容
                // 实际上strtok 函数用于将字符串分割成子字符串，
                // 并不会为每个子字符串分配新的空间。相反，它会直接修改原始字符串，
                // 将分隔符替换为 null 字符（'\0'），从而将字符串分割为多个子字符串。
                // 这边使用一个全局变量env来维护环境变量所需的内容而不是局部变量commandLine
                putenv(env[env_cnt++]);
            } else {
                printf("环境变量导入失败，ddsh暂时仅支持导入最多三个环境变量!\n");
            }
        }
    } else if (strcmp(strs[0], "echo") == 0) {
        ret = 1;
        if (strs[1] == NULL) {
            printf("\n");
        } else {
            if (strs[1][0] == '$') {
                // 打印环境变量
                if (strs[1][1] == '?') {
                    printf("%d\n", last_code);
                    last_code = 0;
                } else {
                    char* t_env = getenv(strs[1] + 1);
                    if (t_env) {
                        printf("%s\n", t_env);
                    }else{
                        printf("\n");
                    }
                }
            } else {
                printf("%s\n", strs[1]);
            }
        }
    }
    return ret;
}

void Excute(char* strs[])
{

    // 创建一个子进程进行程序替换
    pid_t id = fork();
    if (id == 0) {
        //        char tmp[SIZE];
        //        strcpy(tmp, strs[0]);
        //        char tmp_1[SIZE] = "/usr/bin/";
        //        strcat(tmp_1, tmp);
        switch (redirType) {
        case NONE_REDIR:
            break;
        case INPUT_REDIR: {
            int fd = open(redirFile, O_RDONLY,0666);
            if (fd < 0) {
                perror("open");
                exit(errno);
            }
            // 重定向文件已经成功打开
            dup2(fd, 0);

        } break;
        case APPEND_REDIR:
        case OUTPUT_REDIR: {
            int flags = O_WRONLY | O_CREAT;
            if (redirType == APPEND_REDIR) {
                flags |= O_APPEND;
            } else {
                flags |= O_TRUNC;
            }
            int fd = open(redirFile, flags);
            if (fd < 0) {
                perror("open");
                exit(errno);
            }
            dup2(fd, 1);
        }
            break;
        default:
            printf("发生错误!!!!\n");
            break;
        }

        execvp(strs[0], strs);
        exit(1);
    }
    int status = 0;
    pid_t rid = waitpid(id, &status, 0); // 阻塞等待
    if (rid <= 0) {
        printf("发生错误\n");
        exit(1);
    } else if (rid == id) {
        last_code = WEXITSTATUS(status);  //获取子进程的退出码
    }
}



int main()
{
    // 输出提示符并获取用户输入命令ls -al
    printf(ANSI_COLOR_GREEN "欢迎来到wby自定义shell----ddsh\n");
    while (true) {
        redirType = NONE_REDIR;
        redirFile = NULL;
        char commandline[SIZE]; // 输入缓冲区
        int n = Interactive(commandline, sizeof(commandline));
        if (!n) {
            continue;
        }
        if (strlen(commandline) == 1) {
            // 这是一个空串
            continue;
        }
        // 获取字符串之后将其分割成多个子串
        char* strs[SIZE];
        Split(commandline, strs);
        // 处理内建命令
        n = BuildingCmd(strs);
        // 执行命令
        if (n) {
            continue;
        }
        Excute(strs);
    }
    return 0;
}











