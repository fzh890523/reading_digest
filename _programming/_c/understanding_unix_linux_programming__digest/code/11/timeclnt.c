#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>

#define oops(msg) {perror(msg); exit(1);}
#define BUFSIZE 100
#define HOSTLEN 256

int main(int ac, char *av[])
{
  struct sockaddr_in saddr;
  struct hostent *hp;
  char hostname[HOSTLEN];
  int sock_id, sock_fd;
  char message[BUFSIZE];
  int messlen;

  if((sock_id = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
    oops("socket");
  }

  bzero((void *)&saddr, sizeof(saddr));
  if((hp = gethostbyname(av[1])) == NULL) oops(av[1]);
  bcopy(hp->h_addr, (struct sockaddr *)&saddr.sin_addr, hp->h_length);
  saddr.sin_port = htons(atoi(av[2]));
  saddr.sin_family = AF_INET;

  if(connect(sock_id, (struct sockaddr *)&saddr, sizeof(saddr)) != 0) oops("connect");

  messlen = read(sock_id, message, BUFSIZE);
  if(messlen == -1) oops("read");
  if(write(1, message, messlen) != messlen) oops("write");
  close(sock_id);
}
