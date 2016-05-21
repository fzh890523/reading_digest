

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFSIZE 100
#define oops(m, x) {perror(m); exit(x);}

int make_dgram_server_socket(int);
int get_internet_address(char *, int, int *, struct sockaddr_in *);
void say_who_called(struct sockaddr_in *);

int main(int ac, char *av[])
{
  int port, sock;
  char buf[BUFSIZE];
  size_t msglen;
  struct sockaddr_in saddr;
  socklen_t saddrlen;

  if(ac == 1 || (port = atoi(av[1])) <= 0) {
    fprintf(stderr, "usage: dgrecv portnumber\n");
    exit(1);
  }

  if((sock = make_dgram_server_socket(port)) == -1) oops("can not make socket", 2);

  saddrlen = sizeof(saddr);
  while((msglen = recvfrom(sock, buf, BUFSIZE, 0, &saddr, &saddrlen)) > 0){
/*
dgrecv.c: In function ‘main’:
dgrecv.c:31:3: warning: passing argument 5 of ‘recvfrom’ from incompatible pointer type [enabled by default]
   while((msglen = recvfrom(sock, buf, BUFSIZE, 0, &saddr, &saddrlen)) > 0){
   ^
In file included from dgrecv.c:5:0:
/usr/include/sys/socket.h:175:16: note: expected ‘struct sockaddr * __restrict__’ but argument is of type ‘struct sockaddr_in *’
 extern ssize_t recvfrom (int __fd, void *__restrict __buf, size_t __n,

 似乎： sockaddr_in指socket internet addr 而 sockaddr指socket addr，为什么不报错只是warning就不知道了 = =
*/
    buf[msglen] = '\0';
    printf("dgrecv: got a message: %s\n", buf);
    say_who_called(&saddr);
    reply_to_sender(sock, buf, &saddr, saddrlen);
  }
  return 0;
}

void say_who_called(struct sockaddr_in * addrp)
{
  char host[BUFSIZE];
  int port;

  get_internet_address(host, BUFSIZE, &port, addrp);
  printf("from: %s: %d\n", host, port);
}

void reply_to_sender(int sock, char *msg, struct sockaddr_in *addrp, socklen_t len)
{
  char rep,y[BUFSIZE + BUFSIZE];
  sprintf(reply, "Thanks for your %d char message\n", strlen(msg));
  sendto(sock, reply, strlen(reply), 0, addrp, len);
}
