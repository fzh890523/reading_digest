#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <strings.h>

#define PORTNUM 13000
#define HOSTLEN 256
#define oops(msg) {perror(msg); exit(1);}

int main(int ac, char *av[])
{
  struct sockaddr_in saddr;
  struct hostent *hp;
  char hostname[HOSTLEN];
  int sock_id, sock_fd;
  FILE * sock_fp;
  char *ctime();
  time_t thetime;

  // socket domain, type, protocol
  if((sock_id = socket(PF_INET, SOCK_STREAM, 0)) == -1) oops("socket");

  // zero-init
  bzero((void *)&saddr, sizeof(saddr));

// struct hostent {
//     char  *h_name;            /* official name of host */
//     char **h_aliases;         /* alias list */
//     int    h_addrtype;        /* host address type */
//     int    h_length;          /* length of address */
//     char **h_addr_list;       /* list of addresses */
// }
// #define h_addr h_addr_list[0] /* for backward compatibility */

  // get host and construct addr
  gethostname(hostname, HOSTLEN);
  hp = gethostbyname(hostname);
  bcopy((void *)hp->h_addr, (void *)&saddr.sin_addr, hp->h_length);
  saddr.sin_port = htons(PORTNUM);
  saddr.sin_family = AF_INET;
  // printf("saddr.sin_addr is %s\n", saddr.sin_addr); 还不是个字符串，或者说起码不是标准字符串

  // bind sockfd, addr, addrlen
  if(bind(sock_id, (struct sockaddr *)&saddr, sizeof(saddr)) != 0) oops("bind");

  // listen sockfd, backlog
  if(listen(sock_id, 1) != 0) oops("listen");

  while(1){
    // accept
    sock_fd = accept(sock_id, NULL, NULL);
    if(sock_fd == -1) oops("accept");
    printf("Wow! got a call! \n");

    // write
    if((sock_fp = fdopen(sock_fd, "w")) == NULL) oops("fdopen");
    thetime = time(NULL);
    fprintf(sock_fp, "The time here is ...");
    fprintf(sock_fp, "%s", ctime(&thetime));
    fclose(sock_fp);
  }
}
