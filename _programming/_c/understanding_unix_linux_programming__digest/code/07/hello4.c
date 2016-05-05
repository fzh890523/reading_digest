#include <stdio.h>
#include <curses.h>

int main()
{
  int i;
  initscr();
  clear();
  for(i=0; i<LINES; i++){
      move(i, i+i);
      if(i%2 == 1)
        standout();
      addstr("hello, world");
      if(i%2 == 1)
        standend();
      refresh();
      sleep(1);
      move(i, i+i);  // 移动到开头位置
      addstr("            ");  // 等于清空显示字符（空白字符覆盖）
  }
  endwin();
}

/*
需要： gcc -lcurses hello1.c
*/
