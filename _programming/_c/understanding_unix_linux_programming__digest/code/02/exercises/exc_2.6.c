#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define S "testing 123..."

int main()
{
  int fd_rd = open("exc_2.6.c", O_RDONLY);
  int fd_wr = open("exc_2.6.c", O_WRONLY);
  int fd_rd1 = open("exc_2.6.c", O_RDONLY);
  printf("fd_rd is %d; fd_wr is %d; fd_rd1 is %d\n", fd_rd, fd_wr, fd_rd1);
  // ignore error handling

  static const char *s = S;
  // int l = sizeof(s);  // 得到的是指针的长度 = =
  int l = strlen(s);   // 不包含最后的\0
  // 用宏定义的方式可以避免重复字符串
  int sz = sizeof S;  // 包含最后的\0
  printf("strlen of s is %d, size is %d\n", l, sz);
  char buf[20];
  read(fd_rd, buf, 20);
  printf("read 20 from fd_rd: %s\n", buf);  // 有坑，如果第一行（不含\n）小于20的话ok，>=20的话药丸。
  write(fd_wr, s, l);
  read(fd_rd1, buf, 20);
  printf("read 20 from fd_rd1: %s\n", buf);

/*
output:
fd_rd is 3; fd_wr is 4; fd_rd1 is 5
read 20 from fd_rd: #include <stdio.h>
read 20 from fd_rd1: testing  <stdio.h>

同一个进程可以重复打开一个文件，包括wr、rd...
offset在FILE内，而fd - FILE，所以...
*/
}
