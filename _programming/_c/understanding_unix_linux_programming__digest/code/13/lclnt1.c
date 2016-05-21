#include <stdlib.h>
#include <stdio.h>

void do_regular_work();
void shut_down();
void release_ticket();

int main(int ac, char *av[])
{
  setup();
  if(get_ticket() != 0) exit(0);
  do_regular_work();
  release_ticket();
  shut_down():
}

void do_regular_work()
{
  printf("SuperSleep version 1.0 Running - Licensed Software\n");
  sleep(10);
}
