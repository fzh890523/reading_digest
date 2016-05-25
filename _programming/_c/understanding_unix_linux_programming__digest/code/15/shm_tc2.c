#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define TIME_MEM_KEY 99
#define TIME_SEM_KEY 9900
#define SEG_SIZE ((size_t)100)
#define oops(m, x) {perror(m); exit(x);}

union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
};

int main()
{
  int seg_id;
  char *mem_ptr, * ctime();
  long now;

  int semset_id;

  // client就不加 IPC_CREAT 的flag了
  if((seg_id = shmget(TIME_MEM_KEY, SEG_SIZE, 0777)) == -1) oops("shmget", 1);

  if((mem_ptr = (char *)shmat(seg_id, NULL, 0)) == (void *)-1) oops("shmat", 2);

  semset_id = semget(TIME_SEM_KEY, 2, 0);
  wait_and_lock(semset_id);

  printf("the time, direct from memory: .. %s\n", mem_ptr);

  release_lock(semset_id);
  shmdt(mem_ptr);
}

void wait_and_lock(int semset_id)
{
  // 含义是： 当sem_num == 1（这里是write）的sem值为0时对sem_num == 0（这里是read）的sem做+1操作

  struct sembuf actions[2];
  actions[0].sem_num = 1;  // 注意该值，对sem的操作与server相反
  actions[0].sem_flag = SEM_UNDO;
  actions[0].sem_op = 0;

  actions[1].sem_num = 0;
  actions[1].sem_flag = SEM_UNDO;
  actions[1].sem_op = 1;

  if(semop(semset_id, actions, 2) == -1) oops("semop: locking", 10);
}

void release_lock(int semset_id)
{
  struct sembuf actions[1];

  actions[0].sem_num = 0;  // sem_num == 0（read）的sem做-1操作
  actions[0].sem_flag = SEM_UNDO;
  actions[0].sem_op = -1;

  if(semop(semset_id, actions, 1) == -1) oops("semop: unlocking", 10);
}
