#include <stdio.h>
#include <stdlib.h>
#include <utmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define SHOWHOST
#define NRECS 16
#define NULLUT ((struct utmp *)NULL)
#define UTSIZE (sizeof(struct utmp))

static char utmpbuf[NRECS * UTSIZE];
static int num_recs;
static int cur_rec;
static int fd_utmp = -1;

void show_info(struct utmp *);
char *get_time_str(long);
struct utmp *utmp_next();
int utmp_reload();
void utmp_close();

int utmp_open(char *filename)
{
  fd_utmp = open(filename, O_RDONLY);
  cur_rec = num_recs = 0;
  return fd_utmp;
}

void utmp_close()
{
  if(fd_utmp != -1)
    close(fd_utmp);
}

struct utmp *utmp_next()
{
  struct utmp *recp;

  if(fd_utmp == -1)
    return NULLUT;
  if(cur_rec == num_recs && utmp_reload() == 0)
    return NULLUT;

  recp = (struct utmp *) &utmpbuf[cur_rec *UTSIZE];
  cur_rec++;
  return recp;
}

int utmp_reload()
{
  int amt_read;
  amt_read = read(fd_utmp, utmpbuf, NRECS * UTSIZE);
  num_recs = amt_read / UTSIZE;
  cur_rec = 0;
  return num_recs;
}

int main()
{
  struct utmp *current_record;
  if(utmp_open(UTMP_FILE) == -1) {  // 有趣，竟然可以直接read，说明utmp是直接保存struct的...
    perror(UTMP_FILE);
    /*
    #yonka#
    如果不加include <stdlib.h> 的话，会
    main1.c:17:5: warning: incompatible implicit declaration of built-in function ‘exit’ [enabled by default]
    */
    exit(1);
  }
  while((current_record = utmp_next()) != NULLUT)
    show_info(current_record);
  utmp_close();
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
