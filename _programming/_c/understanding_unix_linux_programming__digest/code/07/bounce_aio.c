#include <stdio.h>
#include <curses.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
// #include <fcntl.h>
#include <aio.h>

#define MESSAGE "HELLO"
#define BLANK "     "

/*
注意需要 -lrt 来链接
*/

int row = 10;
int col = 0;
int dir = 1;
int delay = 200;
int done = 0;

struct aiocb kbcbuf;

int main()
{
  void on_alarm(int);
  void on_input(int);
  void setup_aio_buffer();
  int set_ticker(int);

  initscr();
  crmode();
  noecho();
  clear();

  signal(SIGIO, on_input);
  setup_aio_buffer();
  aio_read(&kbcbuf);

  signal(SIGALRM, on_alarm);
  set_ticker(delay);

  mvaddstr(row, col, MESSAGE);

  while(!done)
    pause();
  endwin();
}

void on_input(int signum)
{
  int c;
  char *cp = (char *) kbcbuf.aio_buf;
  if(aio_error(&kbcbuf) != 0)
    perror("reading failed");
  else if(aio_return(&kbcbuf) == 1){
      c = *cp;
      if(c == 'Q' || c == EOF)
        done = 1;
      else if(c == ' ')
        dir = -dir;
  }
  aio_read(&kbcbuf);
}

void on_alarm(int signum)
{
  signal(SIGALRM, on_alarm);
  mvaddstr(row, col, BLANK);
  col += dir;
  mvaddstr(row, col, MESSAGE);
  refresh();

  if(dir == -1 && col <= 0)
    dir = 1;
  else if(dir == 1 && col + strlen(MESSAGE) >= COLS)
    dir = -1;
}

void setup_aio_buffer()
{
  static char input[1];

  kbcbuf.aio_fildes = 0;
  kbcbuf.aio_buf = input;
  kbcbuf.aio_nbytes = 1;
  kbcbuf.aio_offset = 0;

  kbcbuf.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
  kbcbuf.aio_sigevent.sigev_signo = SIGIO;
}

int set_ticker(int n_msecs)
{
  struct itimerval new_timeset;
  long n_sec, n_usecs;

  n_sec = n_msecs / 1000;
  n_usecs = (n_msecs % 1000) * 1000L;

  new_timeset.it_interval.tv_sec = n_sec;
  new_timeset.it_interval.tv_usec = n_usecs;
  new_timeset.it_value.tv_sec = n_sec;
  new_timeset.it_value.tv_usec = n_usecs;

  return setitimer(ITIMER_REAL, &new_timeset, NULL);
}
