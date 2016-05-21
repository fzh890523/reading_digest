#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

static int pid = -1;
static int sd = -1;
static struct sockaddr serv_addr;
static socklen_t serv_alen;
static char ticket_buf[128];
static have_ticket = 0;

#define MSGLEN 128
#define SERVER_PORTNUM 2020
#define HOSTLEN 512
#define oops(p) {perror(p); exit(1);}

void char * do_transaction();

void setup()
{
  char hostname[BUFSIZE];
  pid = getpid();
  if((sd = make_dgram_client_socket()) == -1) oops("can not create socket");
  gethostname(hostname, HOSTLEN);
  make_internet_address(hostname, SERVER_PORTNUM, &serv_addr);
  serv_alen = sizeof(serv_addr);
}

void shut_down()
{
  close(sd);
}

int get_ticket()
{
  char *response;
  char buf[MSGLEN];
  if(have_ticket) return(0);
  sprintf(buf, "HELLO %d", pid);
  if((response = do_transaction(buf)) == NULL) return(-1);

  if(strcmp(response, "TICK", 4) == 0){
    strcpy(ticket_buf, response + 5);
    have_ticket = 1;
    narrate("got ticket", ticket_buf);
    return(0);
  }

  if(strncmp(response, "FAIL", 4) == 0) narrate("could not get ticket", response);
  else narrate("unknown message:", response);
  return(-1);
}

int release_ticket()
{

}

char * do_transaction(char *msg)
{

}

void narrate(char * msg1, char * msg2)
{
  fprintf(stderr, "CLIENT[%d]: %s %s\n", pid, msg1, msg2);
}

void syserr(char * msg1)
{
  char buf[MSGLEN];
  sprintf(buf, "CLIENT[%d]: %s", pid, msg1);
  perror(buf);
}
