#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

#define QUESTION "Do you want another transaction"
#ifndef MIN
  #define MIN 1
#endif
#define TRIES 3
#define SLEEPTIME 2
#define BEEP putchar('\a')

int get_response(char *question, int maxtries);
void set_crmode();
void tty_mode(int how);
void set_nodelay_mode();
void ctrl_c_handler(int signum);

int main()
{
  int response;
  tty_mode(0);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGINT, ctrl_c_handler);
  set_crmode();
  set_nodelay_mode();
  response = get_response(QUESTION, TRIES);
  tty_mode(1);
  return response;
}

int get_response(char *question, int maxtries)
{
  int input;
  printf("%s (y/n)?", question);
  fflush(stdout);
  while (1) {
    sleep(SLEEPTIME);  // 影响体验： 用户的输入可能不能及时响应 - 考虑select
    input = tolower(get_ok_char());
    if(input == 'y')
      return 0;
    if (input == 'n')
      return 1;
    if (maxtries-- == 0)
      return 2;
    BEEP;
  }
}

int get_ok_char()
{
  int c;
  while((c = getchar()) != EOF && strchr("yYnN", c) == NULL);
  return c;
}

void set_crmode()
{
  struct termios ttystate;
  tcgetattr(0, &ttystate);
  ttystate.c_lflag &= ~ICANON;
  ttystate.c_lflag &= ~ECHO;
  ttystate.c_cc[VMIN] = MIN;
  // VMIN为flush的buf大小
  // 为1表示每个字符都flush一次，也即逐字符flush/read/write
  // 0会造成read时如果buf无内容则立即返回EOF（有内容还是读内容） - 也即无阻塞
  tcsetattr(0, TCSANOW, &ttystate);
}

void tty_mode(int how)
{
  static struct termios original_mode;
  static int original_falgs;
  if(how == 0) {
    tcgetattr(0, &original_mode);
    original_falgs = fcntl(0, F_GETFL);
  } else {
    tcsetattr(0, TCSANOW, &original_mode);
    fcntl(0, F_SETFL, original_falgs);
  }
}

void set_nodelay_mode()
{
  int termflags;
  termflags = fcntl(0, F_GETFL);
  termflags |= O_NDELAY;
  fcntl(0, F_SETFL, termflags);
}

void ctrl_c_handler(int signum)
{
  tty_mode(1);
  exit(1);
}
