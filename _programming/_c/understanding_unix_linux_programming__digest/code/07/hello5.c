#include <stdio.h>
#include <curses.h>

#define LEFTEDGE 10
#define RIGHTEDGE 30
#define ROW 10

int main()
{
  char *message = "hello";
  char *blank = "     ";
  int dir = 1;
  int pos = LEFTEDGE;
  initscr();
  clear();
  while(1){
      move(ROW, pos);
      addstr(message);
      move(LINES - 1, COLS - 1);
      refresh();
      sleep(1);
      move(ROW, pos);
      addstr(blank);
      pos += dir;
      if(pos >= RIGHTEDGE)
        dir = -1;
      if(pos <= LEFTEDGE)
        dir = 1;
  }
}

/*
需要： gcc -lcurses hello5.c

问题：
sleep 1s时间过长运动过慢，需要更细粒度（小）的sleep
*/
