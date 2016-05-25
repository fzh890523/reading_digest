#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <time.h>

#define TIME_MEM_KEY 99
#define SEG_SIZE ((size_t)100)
#define oops(m, x) {perror(m); exit(x);}

int main()
{
  int seg_id;
  char *mem_ptr, * ctime();
  long now;
  int n;

  // client就不加 IPC_CREAT 的flag了
  if((seg_id = shmget(TIME_MEM_KEY, SEG_SIZE, 0777)) == -1) oops("shmget", 1);

  if((mem_ptr = (char *)shmat(seg_id, NULL, 0)) == (void *)-1) oops("shmat", 2);

  printf("the time, direct from memory: .. %s\n", mem_ptr);

  shmdt(mem_ptr);
}
