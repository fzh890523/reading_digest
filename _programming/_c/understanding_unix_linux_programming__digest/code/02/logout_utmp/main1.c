#include <utmp.h>
#include <stdlib.h>
#include <stdio.h>

/*
lseek()

SYNOPSIS
       #include <sys/types.h>
       #include <unistd.h>

       off_t lseek(int fd, off_t offset, int whence);

减少系统调用来提升性能： batch-exec、buf
syscall的开销相比external-IO还是好多了，所以内核buf/cache比directIO好（多数情况下）
*/

int logout_tty(char *line)
{
  int fd;
  struct utmp rec;
  int len = sizeof(struct utmp);
  int retval = -1;

  if((fd = open(UTMP_FILE, O_RDONLY)) == -1)
    return -1;

  while(read(fd, &rec, len) == len){  // 粗暴，直接...
    if(strcmp(rec.ut_line, line, sizeof(rec.ut_line)) == 0){
      rec.ut_type = DEAD_PROCESS;
      if(time(&rec.ut_time != -1)){
          if(lseek(fd, -len, SEEK_CUR) != -1){  // 倒回指定...
            if(write(fd, &rec, len))  // 粗暴写回
              retval = 0;
          }
      }
    }
    break;
  }
  if(close(fd) == -1)
  retval = -1;
  return retval;
}
