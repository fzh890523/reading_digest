
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/syscall.h>
#include <string.h>
#include <fcntl.h>

#define COUNT_FILES_BUFSIZE 5120

void do_ls(char dirname[]);
void do_sort_ls(char dirname[]);
int count_files(int dir_fd);

// 用buf性能好一些
static char buf[COUNT_FILES_BUFSIZE];  // 测试用，所以共用buf

int main(int ac, char *av[])
{
  if(ac == 1)
    do_ls(".");
  else {
      while(--ac) {
        printf("%s: \n", * ++av);
        do_ls(*av);
      }
  }
}

void do_ls(char dirname[])
{
  DIR *dir_ptr;
  struct dirent *direntp;
  if((dir_ptr = opendir(dirname)) == (DIR *)NULL)  // 不做转换的话： warning: comparison between pointer and integer
    fprintf(stderr, "ls: cannot open %s\n", dirname);
  else {
    while((direntp = readdir(dir_ptr)) != NULL)
      printf("%s\n", direntp->d_name);
    closedir(dir_ptr);
    // 这里可以考虑对文件名做排序，不过问题是不知道长度... 要考虑数组扩容问题，略囧。
    // getdents 可以一次性全部取出放到给的buf中并且返回数量 --- 错了，因为buf有限，所以每次只会返回buf能存入的数量，要for()来遍历直到..。
    // 好像获取数量只能遍历+计数，哎...
  }
}

void do_sort_ls(char dirname[])
{
  int dir_fd;
  int dir_files_num;
  struct dirent *direntp;
  if((dir_fd = open(dirname, O_RDONLY | O_DIRECTORY)) == -1)
    fprintf(stderr, "ls: cannot open %s\n", dirname);
  else {
    dir_files_num = count_files(dir_fd);
    if(dir_files_num == -1) {
      printf("count_files failed\n");
      close(dir_fd);
      exit(1);
    }
    int nread;
    char **filenames = malloc(dir_files_num * sizeof(char*));
    int cur_index, pos;
    while((nread = syscall(SYS_getdents, dir_fd, buf, COUNT_FILES_BUFSIZE)) > 0) {
      direntp = (struct dirent *)(buf+pos);
      filenames[cur_index++] = direntp->d_name;  // TODO 估计分配在buf里会回收然后就BOOM... 如果数量比较少的话，可能一次buf读完，那就“看起来可以”了，但不代表OK
    }
    if(nread == -1) {
      printf("getdents failed\n");
      close(dir_fd);
      exit(1);
    }
    qsort(filenames, dir_files_num, sizeof(char *), (__compar_fn_t)strcmp);
    /*
    不转的话会：
    main1.c: In function ‘do_sort_ls’:
    main1.c:72:5: warning: passing argument 4 of ‘qsort’ from incompatible pointer type [enabled by default]
         qsort(filenames, dir_files_num, sizeof(char *), strcmp);
         ^
    In file included from main1.c:2:0:
    /usr/include/stdlib.h:760:13: note: expected ‘__compar_fn_t’ but argument is of type ‘int (*)(const char *, const char *)’
     extern void qsort (void *__base, size_t __nmemb, size_t __size,
                 ^
   qsort的example里这么用：
                 static int
                 cmpstringp(const void *p1, const void *p2)
                 {
                     // The actual arguments to this function are "pointers to
                      //  pointers to char", but strcmp(3) arguments are "pointers
                      //  to char", hence the following cast plus dereference

                     return strcmp(* (char * const *) p1, * (char * const *) p2);
                 }
    */
    int i=0;
    for(; i<dir_files_num; i++) {  // error: ‘for’ loop initial declarations are only allowed in C99 mode
      printf("%s\n", filenames[i]);
    }
    close(dir_fd);
  }
}

int count_files(int dir_fd)
{
    int count = 0;
    int nread;
    struct dirent *direntp;
    while((nread = syscall(SYS_getdents, dir_fd, buf, COUNT_FILES_BUFSIZE)) > 0) {
      count += 1;
    }
    if(nread == 0) {
      return count;
    } else {  // -1
      return -1;
    }
}

/*
output:
$ ./a.out /
/:
.
..
boot
dev
proc
run
sys
etc
root
tmp
var
usr
bin
sbin
lib
lib64
home
media
mnt
opt
srv
zookeeper_server.pid
data
*/
