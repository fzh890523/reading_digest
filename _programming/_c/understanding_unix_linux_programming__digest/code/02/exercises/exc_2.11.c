
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#ifndef BUFFERSIZE
  #define BUFFERSIZE 4096
#endif
#define COPYMODE 0644

/*
open syscall

NAME
       open, creat - open and possibly create a file or device

SYNOPSIS
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>

       int open(const char *pathname, int flags);
       int open(const char *pathname, int flags, mode_t mode);

       int creat(const char *pathname, mode_t mode);


creat in glibc
int
creat (const char *file, mode_t mode)
{
  return __open (file, O_WRONLY | O_CREAT | O_TRUNC, mode);
}

*/

/*
benchmark

59M的东西（DST file已有）
BUFFERSIZE CPU
512 0.01s user 0.14s system 99% cpu 0.148 total
1024  0.01s user 0.11s system 99% cpu 0.115 total
5120  0.00s user 0.06s system 99% cpu 0.060 total
51200 0.00s user 0.07s system 99% cpu 0.067 total
128000  0.00s user 0.05s system 99% cpu 0.050 total
*/

void oops(char *, char *);

int main(int ac, char *av[])
{
  int in_fd, out_fd, n_chars;
  char buf[BUFFERSIZE];

  if(ac != 3){
    fprintf(stderr, "usage: %s source destination\n", *av);
    exit(1);
  }

  if((in_fd=open(av[1], O_RDONLY)) == -1)
    oops("can not open ", av[1]);
  if((out_fd=creat(av[2], COPYMODE)) == -1)  // creat == O_WRONLY|O_CREAT|O_TRUNC，所以如果source和dst是同一个文件的话那就悲剧了
    oops("can not create ", av[2]);

  while((n_chars=read(in_fd, buf, BUFFERSIZE)) > 0){
    if(write(out_fd, buf, n_chars) != n_chars)  // in case disk-no-space or quota-outage, etc...
      oops("write error to ", av[2]);
  }
  if(n_chars == -1)
    oops("read error from ", av[1]);

  if(close(in_fd) == -1 || close(out_fd) == -1) oops("error closing files", "");
}

void oops(char *s1, char *s2)
{
  fprintf(stderr, "Error: %s\n", s1);
  perror(s2);
  exit(1);
}
