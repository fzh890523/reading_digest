#include <stdio.h>
#include <stdlib.h>
#include <utmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define SHOWHOST

void show_info(struct utmp *);
char *get_time_str(long);

int main()
{
  struct utmp current_record;
  int utmpfd;
  int reclen = sizeof(current_record);
  if((utmpfd = open(UTMP_FILE, O_RDONLY)) == -1) {  // 有趣，竟然可以直接read，说明utmp是直接保存struct的...
    perror(UTMP_FILE);
    /*
    #yonka#
    如果不加include <stdlib.h> 的话，会
    main1.c:17:5: warning: incompatible implicit declaration of built-in function ‘exit’ [enabled by default]
    */
    exit(1);
  }
  while(read(utmpfd, &current_record, reclen)) show_info(&current_record);
  close(utmpfd);
  return 0;
}

void show_info(struct utmp *utbufp)
{
  if(utbufp->ut_type != USER_PROCESS) return;
  printf("%-8.8s", utbufp->ut_name);
  printf(" ");
  printf("%-8.8s", utbufp->ut_line);
  printf(" ");
  printf(get_time_str(utbufp->ut_time));
  printf(" ");
#ifdef SHOWHOST
  printf("(%s)", utbufp->ut_host);
#endif
  printf("\n");
}

char *get_time_str(long timeval)
{
  char *cp, *str;
  if((str = malloc(100)) == NULL) {  // #yonka# 需要...
    exit(1);
  }
  cp = ctime(&timeval);
  sprintf(str, "%12.12s", cp+4);
  return str;
}
