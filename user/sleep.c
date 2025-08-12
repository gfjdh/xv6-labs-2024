#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
int main(int argc, char *argv[]) {
    if (argc != 2) {
        // 参数数量不对，打印错误信息到标准错误（文件描述符2）
        fprintf(2, "Usage: sleep <ticks>\n");
        exit(1);
    }
    int ticks = atoi(argv[1]); // 将字符串参数转换为整数
    sleep(ticks);              // 调用系统调用sleep
    exit(0);
}
