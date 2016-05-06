
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define INPUTLEN 100

void inthandler(int);
void quithandler(int);

int main()
{
  char input[INPUTLEN];
  int nchars;

  signal(SIGINT, inthandler);
  signal(SIGQUIT, quithandler);
  do {
    printf("\nType a message\n");
    nchars = read(0, input, (INPUTLEN - 1));
    if(nchars == -1)
      perror("read returned an error");
    else {
      input[nchars] = '\0';
      printf("You typed: %s\n", input);
    }
  } while (strncmp(input, "quit", 4) != 0);
}

void inthandler(int signum)
{
  printf("Received signal %d .. waiting\n", signum);
  sleep(2);
  printf("Leaving inthandler\n");
}

void quithandler(int signum)
{
  printf("Received signal %d .. waiting\n", signum);
  sleep(2);
  printf("Leaving quithandler\n");
}



/*
output:
^C 表示输入ctrl-C
^\ 表示输入ctrl-\
2 为 SIGINT
3 为 SIGQUIT

可以看到，处理SIGINT时会屏蔽SIGINT，处理SIGQUIT时同理；
但处理一种信号时不会屏蔽另一种。
同时也可以看到，先收到信号A然后在处理信号A的时候收到信号B的话会跳到执行B的handler，结束后才继续执行A的handler（剩余语句）

# root @ localhost in /tmp [10:53:36] tty:pts/0 L:1 N:240
$ ./a.out

Type a message
^CReceived signal 2 .. waiting
^C^C^C^C^C^C^C^C^C^CLeaving inthandler
Received signal 2 .. waiting
^C^C^C^C^C^C^C^C^C^CLeaving inthandler
Received signal 2 .. waiting
^C^C^C^C^C^C^C^C^C^C^CLeaving inthandler
Received signal 2 .. waiting
^C^C^C^C^C^C^C^C^C^CLeaving inthandler
Received signal 2 .. waiting
^C^C^C^C^C^C^C^C^CLeaving inthandler
Received signal 2 .. waiting
^C^C^C^C^C^C^C^C^C^CLeaving inthandler
Received signal 2 .. waiting
^C^CLeaving inthandler
Received signal 2 .. waiting
Leaving inthandler
^\Received signal 3 .. waiting
Leaving quithandler
^\Received signal 3 .. waiting
^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\Leaving quithandler
Received signal 3 .. waiting
^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\^\Leaving quithandler
Received signal 3 .. waiting
Leaving quithandler
^CReceived signal 2 .. waiting
^\Received signal 3 .. waiting
Leaving quithandler
Leaving inthandler
^\Received signal 3 .. waiting
^CReceived signal 2 .. waiting
^\^CLeaving inthandler
Received signal 2 .. waiting
^\^CLeaving inthandler
Received signal 2 .. waiting
Leaving inthandler
Leaving quithandler
Received signal 3 .. waiting
^\^CReceived signal 2 .. waiting
Leaving inthandler
Leaving quithandler
Received signal 3 .. waiting
Leaving quithandler
^\Received signal 3 .. waiting
^CReceived signal 2 .. waiting
Leaving inthandler
Leaving quithandler
^\Received signal 3 .. waiting
^CReceived signal 2 .. waiting
Leaving inthandler
Leaving quithandler
quit
You typed: quit
*/
