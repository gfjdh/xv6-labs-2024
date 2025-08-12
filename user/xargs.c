#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define BUFSIZE 512  // 输入缓冲区大小

// 执行命令函数：创建子进程执行命令数组args中的命令
void execute(char *args[]) {
    if (fork() == 0) {  // 子进程
        exec(args[0], args);  // 执行命令
        fprintf(2, "exec %s failed\n", args[0]);  // 执行失败报错
        exit(1);
    }
    wait(0);  // 父进程等待子进程完成
}

int main(int argc, char *argv[]) {
    char *args[MAXARG];  // 命令参数数组（最大参数数由MAXARG定义）
    char buf[BUFSIZE];   // 输入缓冲区
    int n, arg_count;    // n: 读取字节数, arg_count: 当前参数计数

    // 参数检查：至少需要指定一个命令
    if (argc < 2) {
        fprintf(2, "Usage: xargs command [initial-arguments...]\n");
        exit(1);
    }

    // 复制初始命令和参数（跳过xargs自身名称）
    for (int i = 1; i < argc; i++) {
        args[i - 1] = argv[i];  // args[0] = argv[1], args[1] = argv[2]...
    }
    arg_count = argc - 1;  // 当前参数计数 = 初始参数个数

    // 从标准输入循环读取数据
    while ((n = read(0, buf, BUFSIZE)) > 0) {
        char *p = buf;          // 当前解析位置指针
        char *end = buf + n;    // 缓冲区结束位置

        // 遍历缓冲区内容
        while (p < end) {
            char *start = p;    // 当前参数的起始位置
            int end_of_line = 0; // 行结束标志

            // 查找参数结束位置（空格或换行符）
            while (p < end && *p != '\n' && *p != ' ') 
                p++;

            // 找到有效参数（非空字符串）
            if (p > start) {
                // 检查参数数量是否超过上限
                if (arg_count >= MAXARG - 1) {
                    fprintf(2, "xargs: too many arguments\n");
                    exit(1);
                }

                // 如果结束符是换行，标记行结束
                if (*p == '\n') {
                    end_of_line = 1;
                }

                *p = '\0';  // 将结束符替换为字符串终止符

                // 为参数分配内存（长度+1是为了存储终止符）
                args[arg_count] = malloc(p - start + 1);
                if (!args[arg_count]) {
                    fprintf(2, "xargs: malloc failed\n");
                    exit(1);
                }
                strcpy(args[arg_count], start);  // 复制参数内容
                arg_count++;  // 参数计数增加
            }

            // 如果遇到换行符（一行输入结束）
            if (p < end && end_of_line) {
                args[arg_count] = 0;  // 参数数组结尾置NULL
                execute(args);  // 执行命令

                // 释放本行添加的动态参数内存
                while (arg_count > argc - 1) {
                    free(args[--arg_count]);  // 逆序释放并重置计数
                }
                arg_count = argc - 1;  // 重置为初始参数数量
            }
            p++;  // 移动到下一个字符（跳过当前空格/换行）
        }
    }

    // 处理最后一行没有换行符的情况（"dangling"参数）
    if (arg_count > argc - 1) {
        args[arg_count] = 0;  // 参数数组结尾置NULL
        execute(args);  // 执行命令

        // 释放剩余参数内存
        while (arg_count > argc - 1) {
            free(args[--arg_count]);
        }
    }

    exit(0);  // 正常退出
}