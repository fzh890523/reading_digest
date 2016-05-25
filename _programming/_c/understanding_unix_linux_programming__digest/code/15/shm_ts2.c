#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <time.h>
#include <string.h>
#include <sys/sem.h>
#include <signal.h>

#define TIME_MEM_KEY 99
#define TIME_SEM_KEY 9900
#define SEG_SIZE ((size_t)100)
#define oops(m, x) {perror(m); exit(x);}

/*
int semget(key_t key, int nsems, int semflg);
int semop(int semid, struct sembuf *sops, unsigned nsops);
int semctl(int semid, int semnum, int cmd, ...);
*/

union semun {
  int val;
  struct semid_ds *buf;
  ushort *array;
};

int seg_id, semset_id;
void cleanup(int);
void wait_and_lock(int);
void release_lock(int);

int main()
{
  char *mem_ptr, * ctime();
  long now;
  int n;

  if((seg_id = shmget(TIME_MEM_KEY, SEG_SIZE, IPC_CREAT|0777)) == -1) oops("shmget", 1);

  if((mem_ptr = (char *)shmat(seg_id, NULL, 0)) == (void *)-1) oops("shmat", 2);

  semset_id = semget(TIME_SEM_KEY, 2, (0666|IPC_CREAT|IPC_EXCL));
  if(semset_id == -1) oops("semget", 3);
  set_sem_value(semset_id, 0, 0);
  set_sem_value(semset_id, 1, 0);
  signal(SIGINT, cleanup);  // 释放shm sem

  for(n=0; n<60; n++){
    time(&now);
    printf("\tshm_ts2 waiting for lock\n");
    wait_and_lock(semset_id);
    printf("\tshm_ts2 updating memory\n");
    strcpy(mem_ptr, ctime(&now));
    sleep(5);
    release_lock(semset_id);
    printf("\tshm_ts2 released lock\n");
    sleep(1);
  }

  cleanup(0);
}

void cleanup(int n)
{
  shmctl(seg_id, IPC_RMID, NULL);
  semctl(semset_id, 0, IPC_RMID, NULL);
}

void set_sem_value(int semset_id, int sennum, int val)
{
  union semun initval;
  initval.val = val;
  if(semctl(semset_id, semnum, SETVAL, initval) == -1) oops("semctl", 4);
}

void wait_and_lock(int semset_id)
{
  // 含义是： 当sem_num == 0（这里是read）的sem值为0时对sem_num == 1（这里是write）的sem做+1操作

  struct sembuf actions[2];
  actions[0].sem_num = 0;
  actions[0].sem_flag = SEM_UNDO;  // 表示在进程退出的时候内核会恢复该操作（以免因为某进程的意外退出而使得其他进程“饥饿”/“死锁”
  actions[0].sem_op = 0;  // wait-for-zero

  actions[1].sem_num = 1;
  actions[1].sem_flag = SEM_UNDO;
  actions[1].sem_op = +1;  // +1操作

  if(semop(semset_id, actions, 2) == -1) oops("semop: locking", 10);
}

void release_lock(int semset_id)
{
  struct sembuf actions[1];

  actions[0].sem_num = 1;  // sem_num == 1（write）的sem做-1操作
  actions[0].sem_flag = SEM_UNDO;
  actions[0].sem_op = -1;  // -1操作

  if(semop(semset_id, actions, 1) == -1) oops("semop: unlocking", 10);
}
