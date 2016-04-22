
#ifndef BUFSIZE
#define BUFSIZE 512
#endif

/*
1. 文件数： 单个
*/

seek_to_last_n_line(int fd, int line)
{
  
}

int main(int ac, char * av[])
{
  if(ac != 1){
    printf("ac %d is not 1\n", ac);
    exit(1)
  }

  int fd;
  if((fd = open(av[0], O_RDONLY)) == -1){
    printf("open file %s failed\n", av[0]);
  }

  char buf[BUFSIZE + 1];



}
