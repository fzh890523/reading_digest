#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>
#define PAGELEN 24
#define LINELEN 80

/*
相比main2，支持了无需回车读取stdin

FILE * fopen(char *filepath, char *mode);
fclose(FILE * __stream)

fgets
fputs

printf

getchar
getc

termios
tcgetattr/tcsetattr

int fileno(FILE *fp)
*/




int do_more(FILE *, FILE *);
int see_more(FILE *);

int main(int ac, char *av[])
{
  FILE *fp;
  FILE *fp_tty;
  fp_tty = fopen("/dev/tty", "r");
  static struct termios oldt, newt;
  tcgetattr(fileno(fp_tty), &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(fileno(fp_tty), TCSANOW, &newt);
  if(fp_tty == NULL) {
    printf("can not open tty");
    exit(1);
  }
  if(ac == 1)
  	do_more(stdin, fp_tty);
  else {
  	while(--ac) {
  		if((fp=fopen(* ++av, "r")) != NULL) {
  			printf("file: %s\n", av);
        int err_no = do_more(fp, fp_tty);
        fclose(fp);
        if(err_no == ECANCELED) exit(0);
        else if(err_no < 0) exit(1);
  		}
  		else {
  			printf("open file (%s) failed", av);
  			exit(1);
        tcsetattr(fileno(fp_tty), TCSANOW, &oldt);
  		}
  	}
  }
  tcsetattr(fileno(fp_tty), TCSANOW, &oldt);
}

int do_more(FILE *fp, FILE *fp_tty)
{
	char line[LINELEN];
	int num_of_lines = 0;
	int see_more(FILE *), reply;
	while(fgets(line, LINELEN, fp)) {
		if(num_of_lines == PAGELEN) {
			reply = see_more(fp_tty);
			if(reply == 0) return ECANCELED;
			num_of_lines -= reply;
		}
		if(fputs(line, stdout) == EOF) {  // EOF and then check errno; else means success
			printf("fputs(%s, stdout) failed, err_no is %d\n", line, errno);
			return errno;
		}
		fflush(stdout);  // #yonka# necessary
		num_of_lines++;
	}
  return 0;
}

int see_more(FILE *tty)
{
	int c;
	printf("\033[7m more? \033[m");
	while((c=getc(tty)) != EOF){
		if(c == 'q') return 0;
		else if(c == ' ') return PAGELEN;
		else if(c == '\n') return 1;
	}
	return 0;
}
