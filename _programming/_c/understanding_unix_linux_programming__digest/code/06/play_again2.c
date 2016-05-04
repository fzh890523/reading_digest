#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/errno.h>

/*
-ICANON
-ECHO
*/

#define QUESTION "Do you want another transaction"
#ifndef MIN
  #define MIN 1
#endif

int get_response(char *question);
void set_crmode();
void tty_mode(int how);

int main()
{
  int response;
  tty_mode(0);
  set_crmode();
  response = get_response(QUESTION);
  tty_mode(1);
  return response;
}

int get_response(char *question)
{
  int input;
  printf("%s (y/n)?", question);
  while (1) {
    // sleep(2);
    switch (input = getchar()) {
      case 'y':
      case 'Y':
        printf("%d - %c - yes!\n", input, input);
        return 0;
      case 'n':
      case 'N':
      case EOF:
        printf("%d - %c - no!\n", input, input);
        return 1;
      default:
        printf("\ncan not undertand %d - %c, ", input, input);
        printf("Please type y or no \n");
    }
  }
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
  if(how == 0)
    tcgetattr(0, &original_mode);
  else
    tcsetattr(0, TCSANOW, &original_mode);
}
