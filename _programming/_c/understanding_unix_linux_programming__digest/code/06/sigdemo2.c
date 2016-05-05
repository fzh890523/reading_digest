#include <stdio.h>
#include <signal.h>

int main()
{
  void f(int);
  int i;
  signal(SIGINT, SIG_IGN);
  printf("you can not stop me!\n");
  for(i=0; i<5; i++) {
    printf("hello\n");
    sleep(1);
  }
}

void f(int signum)
{
  printf("OUCH! signum is %d\n", signum);
}
