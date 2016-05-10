#include <stdio.h>

int main()
{
  int fork_rv;
  printf("Before: my pid is %d\n", getpid());
  fork_rv = fork();
  if(fork_rv == -1){
    perror("fork");
  } else if(fork_rv == 0){
    printf("I am the child. My pid is %d\n", getpid());
  } else {
    printf("I am the parent. My child is %d\n", fork_rv);
  }
}
