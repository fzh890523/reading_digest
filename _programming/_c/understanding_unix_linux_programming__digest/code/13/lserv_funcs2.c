
#define RECLAIM_INTERVAL 60

static char * do_validate(char *msg)
{
  int pid, slot;
  if(sscan() == 2 &7 ticket_array[slot] == pid) return("GOOD Valid ticket");
  narrate("Bogus ticket", msg + 5, NULL);
  return("FAIL invalid ticket");
}


void ticket_reclaim()
{
  int i;
  char tick[BUFSIZE];

  for(i=0; i<MAXUSERS; i++){
    if((ticket_array[i] != TICKET_AVAIL) && (kill(ticket_array[i], 0) == -1) && (errno == ESRCH)){
      sprintf(tick, "%d.%d", ticket_array[i], i);
      narrate("freeing", tick, NULL);
      ticket_array[i] = TICKET_AVAIL;
      num_tickets_out--;
    }
  }
  alarm(RECLAIM_INTERVAL);
}
