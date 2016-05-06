#include <stdio.h>
#include <signal.h>
#define INPUTLEN 100

int main()
{
  struct sigaction newhandler;
  sigset_t blocked;
  void inthandler();
  char x[INPUTLEN];

  newhandler.sa_handler = inthandler;
  // newhandler.sa_flags = SA_RESETHAND | SA_RESTART;
  newhandler.sa_flags = SA_RESTART;

  sigemptyset(&blocked);
  sigaddset(&blocked, SIGQUIT);
  newhandler.sa_mask = blocked;

  if(sigaction(SIGINT, &newhandler, NULL) == -1)
    perror("sigaction");
  else
    while(1){
      fgets(x, INPUTLEN, stdin);
      printf("input: %s\n", x);
    }
}

void inthandler(int signum)
{
  printf("called with signal %d\n", signum);
  sleep(signum);
  printf("done handling signal %d\n", signum);
}


// SA_RESETHAND 似乎只是不自动reset该信号的handler，行为是阻塞其他SIGX信号然后被handler执行完后被后续该信号...
// 而且，处理SIGX时，后一个SIGX被阻塞，再后的SIGX被IGN了（= =）
/*
^Ccalled with signal 2
^C^C^C^Cdone handling signal 2
called with signal 2
done handling signal 2

input:
*/
