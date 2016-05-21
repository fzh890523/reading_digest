#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


int main()
{
  inf sock, fd;
  void child_waiter(int), process_request(int);
  signal(SIGCHLD, child_waiter);
  if((sock = make_sever_socket(PORTNUM)) == -1) oops("make_sever_socket");

  while(1){
    fd = accept(sock, NULL, NULL);
    if(fd == -1) break;
    process_request(fd);
    close(fd);
  }
}

void child_waiter(int signum)
{
  wait(NULL);  // any child
}

void process_request(int fd)
{
  if(fork() == 0){  // 子进程
    dup2(fd, 1);  // fd作为stdout
    close(fd);
    execlp("data", "date", NULL);
    oops("execlp date");  // 如果子进程能执行到这一步说明execlp失败了
  }
}
