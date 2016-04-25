#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void show_stat_info(char * fname, struct stat * buf);
void mode_to_letters(int mode, char str[]);
char * uid_to_name(uid_t uid);
char * gid_to_name(gid_t gid);

/*

strcpy
stat
*/


int main(int ac, char *av[])
{
  struct stat info;
  if(ac > 1) {
    if(stat(av[1], &info) != -1) {
      show_stat_info(av[1], &info);
      return 0;
    }
  } else {
    perror(av[1]);
  }
  return 1;
}

void show_stat_info(char *fname, struct stat *buf)
{
  printf("mode: %o\n", buf->st_mode);  // 八进制
  printf("links: %d\n", buf->st_nlink);
  printf("user: %d\n", buf->st_uid);
  printf("group: %d\n", buf->st_gid);
  printf("size: %d\n", buf->st_size);
  printf("modtime: %d\n", buf->st_mtime);
  printf("fname: %s\n", fname);
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
  return getpwuid(uid)->pw_name;  // NULL情况
}

char * gid_to_name(gid_t gid)
{
  return getgrgid(gid)->gr_name;  // NULL情况
}
