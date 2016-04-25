
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/syscall.h>
#include <string.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define COUNT_FILES_BUFSIZE 5120

void do_ls(char dirname[]);
void do_sort_ls(char dirname[]);
int count_files(int dir_fd);

void show_file_info(char * fname, struct stat * buf);
void do_stat(char * fname);
void mode_to_letters(int mode, char str[]);
char * uid_to_name(uid_t uid);
char * gid_to_name(gid_t gid);

/*
ctime
*/

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
      do_stat(direntp->d_name);
    closedir(dir_ptr);
    // 这里可以考虑对文件名做排序，不过问题是不知道长度... 要考虑数组扩容问题，略囧。
    // getdents 可以一次性全部取出放到给的buf中并且返回数量 --- 错了，因为buf有限，所以每次只会返回buf能存入的数量，要for()来遍历直到..。
    // 好像获取数量只能遍历+计数，哎...
  }
}

void do_stat(char * fname)
{
  struct stat info;
  if(stat(fname, &info) != -1) {
    show_file_info(fname, &info);
  } else {
    perror(fname);
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
      filenames[cur_index++] = direntp->d_name;  // TODO 估计分配在buf里会回收然后就BOOM... 如果数量比较少的话，可能一次buf读完，那就“看起来可以”了，但不代表OK。 但实际测试，buf设到32都正常，晕死！
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

void show_file_info(char *fname, struct stat *buf)
{
  char modestr[11];
  modestr[10] = '\0';  // 多出的长度应该默认\0吧，这样好像不用...
  mode_to_letters(buf->st_mode, modestr);
  printf("%s", modestr);  // 八进制
  printf("%4d", (int)buf->st_nlink);
  printf("%-8s", uid_to_name(buf->st_uid));
  printf("%-8s", gid_to_name(buf->st_gid));
  printf("%8ld", (long)buf->st_size);
  printf("%.12s", 4 + ctime(&buf->st_mtime));  // FIXME segment err
/*
151       printf("%.12s", 4 + ctime(&buf->st_mtime));
Value returned is $11 = 0x7ffff7dd6dc0 <result> "Tue Apr 26 00:31:18 2016\n"

为什么每次都会
Program received signal SIGSEGV, Segmentation fault.
__strnlen_sse2 () at ../sysdeps/x86_64/strnlen.S:34
34              pcmpeqb (%rdi), %xmm2

而下面的代码没有任何问题
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int main()
{
    time_t t = time(NULL);
    struct stat info;
    stat("main1.c", &info);
    printf("%.12s", ctime(&t));
    printf("\nbetween\n");
    printf("%.12s", ctime(&info.st_mtime));
    printf("\nbetween\n");
    printf("%.12s", 4 + ctime(&t));
    printf("\nbetween\n");
    printf("st_mtime is %d", info.st_mtime);
    printf("\nbetween\n");
    printf("%.12s", 4 + ctime(&info.st_mtime));
    printf("\nend\n");
}
*/
  printf("%s\n", fname);
}

void mode_to_letters(int mode, char str[])  // 长度至少为10
{
  strcpy(str, "----------");
  if(S_ISDIR(mode)) str[0] = 'd';
  if(S_ISCHR(mode)) str[0] = 'c';
  if(S_ISBLK(mode)) str[0] = 'b';

  if(mode & S_IRUSR) str[1] = 'r';
  if(mode & S_IWUSR) str[2] = 'w';
  if(mode & S_IXUSR) str[3] = 'x';

  if(mode & S_IRGRP) str[4] = 'r';
  if(mode & S_IWGRP) str[5] = 'w';
  if(mode & S_IXGRP) str[6] = 'x';

  if(mode & S_IROTH) str[7] = 'r';
  if(mode & S_IWOTH) str[8] = 'w';
  if(mode & S_IXOTH) str[9] = 'x';
}

char * uid_to_name(uid_t uid)
{
  struct passwd *pw_ptr;
  static char numstr[10];
  if((pw_ptr = getpwuid(uid)) == (struct passwd *)NULL) {
    sprintf(numstr, "%d", uid);
    return numstr;
  } else
    return pw_ptr->pw_name;  // NULL情况
}

char * gid_to_name(gid_t gid)
{
  struct group *grp_ptr;
  static char numstr[10];
  if((grp_ptr = getgrgid(gid)) == (struct group *)NULL){
    sprintf(numstr, "%d", gid);
    return numstr;
  }  else
    return grp_ptr->gr_name;  // NULL情况
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
