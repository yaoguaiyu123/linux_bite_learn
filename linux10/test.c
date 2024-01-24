#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>


int main(int argc, char* argv[], char* env[]){
    printf("\n打印环境变量!!!!\n\n");

    for (int i = 0; env[i]; i++) {
        printf("env[%d]: %s\n", i, env[i]);
    }

    return 0;
}
