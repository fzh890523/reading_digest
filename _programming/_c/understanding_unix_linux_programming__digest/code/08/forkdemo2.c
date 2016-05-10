#include <stdio.h>

int main()
{
  printf("Before: my pid is %d\n", getpid());
  fork();
  fork();
  fork();
  printf("After: my pid is %d\n", getpid());
}
