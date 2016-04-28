#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int read_no_follow_link_file();
int read_link_file();
int read_link();

int read_no_follow_link_file()
{
  int fd = open("123.lnk", O_RDONLY | O_NOFOLLOW);
  if(fd == -1){
    perror("open");
    // open: Too many levels of symbolic links
    return -1;
  }
/*
       O_NOFOLLOW
              If pathname is a symbolic link, then the open fails.  This is
              a FreeBSD extension, which was added to Linux in version
              2.1.126.  Symbolic links in earlier components of the pathname
              will still be followed.  See also O_PATH below.
*/
  int read_count;
  char buf[21];
  while((read_count = read(fd, buf, 20)) > 0){
    buf[read_count] = '\0';
    printf("%s", buf);
  }
  if(read_count == -1) {
    perror("read");
    return -1;
  }
}

int read_link_file()
{
  int fd = open("123.lnk", O_RDONLY);
  if(fd == -1){
    perror("open");
    return -1;
  }
  int read_count;
  char buf[21];
  while((read_count = read(fd, buf, 20)) > 0){
    buf[read_count] = '\0';
    printf("%s", buf);
  }
  if(read_count == -1) {
    perror("read");
    return -1;
  }
}

int read_link()
{
  struct stat sb;
  char *linkname;
  ssize_t r;
  if(lstat("123.lnk", &sb) == -1){
    perror("lstat");
    return -1;
  }
  if((linkname = (char *)malloc(sb.st_size + 1)) == (char *)NULL) {
    perror("malloc");
    return -1;
  }
  if((r = readlink("123.lnk", linkname, sb.st_size + 1)) == -1) {
    perror("readlink");
    return -1;
  }else if(r > sb.st_size) {
    perror("readlink increase size");
    return -1;
  }
  linkname[r] = '\0';
  printf("linkname: %s\n", linkname);
  // linkname: /tmp/123 for lrwxrwxrwx. 1 root root 8 Apr 28 23:39 123.lnk -> /tmp/123
  // linkname: 123 for lrwxrwxrwx. 1 root root 3 Apr 28 22:25 123.lnk -> 123
}

int main()
{
  read_link();
  read_no_follow_link_file();
  read_link();
}
