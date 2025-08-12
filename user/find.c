#include "kernel/types.h"   // 包含基本类型定义
#include "kernel/stat.h"    // 包含文件状态结构体定义
#include "user/user.h"      // 包含用户库函数声明
#include "kernel/fs.h"      // 包含文件系统相关定义
#include "kernel/fcntl.h"   // 包含文件控制选项

// find函数：递归查找指定路径下的文件
// path: 当前搜索路径, filename: 要查找的文件名
void find(char *path, char *filename)
{
    char buf[512], *p;      // 缓冲区用于构建完整路径
    int fd;                 // 文件描述符
    struct dirent de;       // 目录项结构
    struct stat st;         // 文件状态结构

    // 尝试打开目录
    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // 获取文件状态
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }   

    // 只处理目录类型
    if (st.type == T_DIR) {
        // 检查路径长度是否超过缓冲区
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
            printf("find: path too long\n");
            return;
        }
        strcpy(buf, path);  // 复制基础路径
        p = buf + strlen(buf);  // 指向路径末尾
        *p++ = '/';         // 添加路径分隔符
        
        // 遍历目录项
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0)  // 跳过空闲目录项
                continue;
                
            // 将目录项名称拼接到路径后
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;  // 确保字符串终止
            
            // 跳过 "." 和 ".." 目录
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;
                
            // 获取完整路径的文件状态
            if (stat(buf, &st) < 0) {
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            
            // 找到目标文件时打印完整路径
            if (strcmp(de.name, filename) == 0) {
                printf("%s\n", buf);
            }
            
            // 如果是子目录则递归查找
            if (st.type == T_DIR) {
                find(buf, filename);
            }
        }
    }
    close(fd);  // 关闭目录
}

int main(int argc, char *argv[])
{
    // 参数检查：需要路径和文件名
    if (argc < 3) {
        fprintf(2, "Usage: find <path> <filename>\n");
        exit(1);
    }

    // 从指定路径开始查找
    find(argv[1], argv[2]);
    exit(0);  // 正常退出
}