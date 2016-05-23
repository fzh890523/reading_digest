#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM 5

int counter = 0;

int main()
{
  pthread t1;
  void *print_count(void *);
  int i;

  pthread_create(&t1, NULL, print_count, NULL);

  for(i=0; i<NUM; i++){
    counter++;
    sleep(1);
  }

  pthread_join(t1, NULL);
}

void * print_count(void *m)
{
    int i;
    for(i=0; i<NUM; i++){
      printf("counter = %d", counter);
      fflush(stdout);
      sleep(1);
    }
    return NULL;
}
