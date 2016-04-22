#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
  char buf[21];
  buf[20] = '\0';
  int fd = open("exc_2.9.c", O_RDWR);
  // END: x
  lseek(fd, 100, SEEK_END);  // pos: x+100
  printf("after lseek 100 from SEEK_END, cur pos is %d\n", lseek(fd, 0, SEEK_CUR));
  int cnt = read(fd, buf, 20);  // pos: x+100，因为超过了END，所以不会读到东西，pos不会变化
  // cnt == 0
  printf("after read 20, cur pos is %d\n", lseek(fd, 0, SEEK_CUR));
  printf("read 20 bytes after SEEK_END, actually read %d, and str is: (%s)\n", cnt, buf);
  lseek(fd, 20000, SEEK_END);  // pos: x+20000 （还原了前面的）
  printf("after lseek 20000 from SEEK_END, cur pos is %d\n", lseek(fd, 0, SEEK_CUR));
  write(fd, "hello", 5);  // pos: x+20000+5，导致SEEK_END之后 和 "hello"之间填充\0产生空洞。 这里是从 369 -> 20374
  printf("after write 5 from SEEK_END, cur pos is %d\n", lseek(fd, 0, SEEK_CUR));
  // 如果只执行最后的lseek 20000+write的话，从 378(加了注释字符) ---> 20383，正好+20000+5...
}
