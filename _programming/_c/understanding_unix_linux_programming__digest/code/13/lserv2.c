

int main(int ac, char *av[])
{
  struct sockaddr client_addr;
  socklen_t addrlen = sizeof(client_addr);
  char buf[MSGLEN];
  int ret, sock;
  void ticket_reclaim();
  unsigned time_left;

  sock = setup();
  signal(SIGALRM, ticket_reclaim);
  alarm(RECLAIM_INTERVAL);

  while(1){
    addrlen = sizeof(client_addr);
    if((ret = recvfrom(sock, buf, MSGLEN, 0, &client_addr, &addrlen)) != -1){
      buf[ret] = '\0';
      narrate("GOT:", buf, &client_addr);
      time_left = alarm(0);
      handle_request(buf, &client_addr, addrlen);
      alarm(time_left);
    } else if(errno != EINTR){
      perror("recvfrom");
    }
  }
}
