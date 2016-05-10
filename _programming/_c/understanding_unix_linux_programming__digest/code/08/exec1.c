#include <stdio.h>

int main()
{
  char *arglist[3];
  arglist[0] = "ls";
  arglist[1] = "-l";
  arglist[2] = 0;  // 额？

  printf("*** About to exec ls -l\n");
  execvp("ls", arglist);  // 等于“换脑”，后面的代码就没有意义了
  printf("*** ls is done. bye\n");
}
