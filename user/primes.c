#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
// 需要声明函数不会返回，否则会警告
void primes(int pipe_parent[2]) __attribute__((noreturn));
void primes(int pipe_parent[2])
{
    int pipe_child[2];
    int prime;
    int n;
    close(pipe_parent[1]);
    // 这时候还没创建pipe_child[0]
    if (read(pipe_parent[0],&prime,sizeof(int)) ==sizeof(int)){
        printf("prime %d\n",prime);
        if (pipe(pipe_child) < 0){
            fprintf(2,"pipe failed\n");
            exit(1);
        }
        if (fork() == 0){// 子进程：关闭写端，递归处理下一级
            close(pipe_parent[0]);//如果不关闭，父进程会阻塞在read
            primes(pipe_child);// 递归调用
            exit(0);
        }
        else{
            close(pipe_child[0]);// 父进程：关闭读端
// 从上游读取数字并过滤
            while (read(pipe_parent[0], &n, sizeof(int)) == sizeof(int))
            {
                if (n % prime != 0){
                    write(pipe_child[1],&n,sizeof(int));
                }
            }
            close(pipe_child[1]);
        }
}
close(pipe_parent[0]);// 关闭管道并等待子进程
    wait(0);
    exit(0);
}
int main(int argc, char *argv[])
{
    int pipe_parent[2];
    if (pipe(pipe_parent) < 0){
        fprintf(2,"pipe failed\n");
        exit(1);
    }
    if (fork() == 0){
        primes(pipe_parent);
    }else{
        close(pipe_parent[0]);
        for (int i = 2; i <= 280; i++){
            write(pipe_parent[1],&i,sizeof(int));
        }
        close(pipe_parent[1]);
        wait(0);
    }
    exit(0);
}