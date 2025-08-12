#include "kernel/types.h"
#include "user/user.h"

int main() {
    // 创建两个管道：parent->child 和 child->parent
    int fd1[2], fd2[2];
    pipe(fd1);
    pipe(fd2);
    
    char buf[1];
    if (fork() == 0) { // 子进程
        close(fd1[1]); // 关闭父进程写端
        close(fd2[0]); // 关闭父进程读端
        
        // 从父进程读取字节
        read(fd1[0], buf, 1);
        printf("%d: received ping\n", getpid());
        
        // 写回字节给父进程
        write(fd2[1], buf, 1);
        
        close(fd1[0]);
        close(fd2[1]);
        exit(0);
    } else { // 父进程
        close(fd1[0]); // 关闭子进程读端
        close(fd2[1]); // 关闭子进程写端
        
        // 发送字节给子进程
        write(fd1[1], "a", 1);
        
        // 等待子进程返回字节
        read(fd2[0], buf, 1);
        printf("%d: received pong\n", getpid());
        
        close(fd1[1]);
        close(fd2[0]);
        wait(0); // 等待子进程退出
        exit(0);
    }
}
