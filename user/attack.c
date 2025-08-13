#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int
main(int argc, char *argv[])
{
  // your code here.  you should write the secret to fd 2 using write
  // (e.g., write(2, secret, 8)
  char *end = sbrk(PGSIZE * 32);
  end = end + 16 * PGSIZE;
  char *secret = end + 32;
  printf("secret: %s\n", secret);
  write(2, secret, 8);
  exit(1);
}

/*
通过在kalloc处打印消息：68行和86行
在secret和attack文件中sbrk之前和之后设置标志。
可以清楚看见kalloc了哪些physical memory
由于程序的其他一些部分也会进行kalloc和kfree，所以attack中的end的virtual address 和secret中的end的virtual address
are not the same.但是他们的physical address相同。
secret中的phycial address可以从
printf("size: %d\n", size);
这一句之后打印的kalloc语句从0数到9找到。

(sbrk return the address of the end of the old memory:
addr = myproc()->sz;
  //if(growproc(n) < 0)
   // return -1;
return addr; sysproc.c 45行)

找到secret中的end的physical address后，
找到printf("attack start\n");
通过kalloc打印的语句，通过end的physical address，找到attack的end的virtual address。需要从零数到16。
这就是attack.c中end + 16 * PGSIZE的来源。
*/