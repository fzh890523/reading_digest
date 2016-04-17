#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#define PAGELEN 24
#define LINELEN 80

/*
FILE * fopen(char *filepath, char *mode);
fclose(FILE * __stream)

fgets
fputs

printf

getchar
*/




void do_more(FILE *);
int see_more();

int main(int ac, char *av[])
{
  FILE *fp;
  if(ac == 1)
  	do_more(stdin);
  else {
	while(--ac) {
		if((fp=fopen(* ++av, "r")) != NULL) {
			printf("file: %s\n", av);
			do_more(fp);
			fclose(fp);
		}
		else {
			printf("open file (%s) failed", av);
			exit(1);
		}
	}
  }
}

void do_more(FILE *fp)
{
	char line[LINELEN];
	int num_of_lines = 0;
	int see_more(), reply;  // #yonka# 这里声明也可以 = =
	while(fgets(line, LINELEN, fp)) {
		if(num_of_lines == PAGELEN) {
			reply = see_more();
			if(reply == 0) break;
			num_of_lines -= reply;
		}
		if(fputs(line, stdout) == EOF) {  // EOF and then check errno; else means success
			printf("fputs(%s, stdout) failed, err_no is %d\n", line, errno);
			exit(1);
		}
		fflush(stdout);  // #yonka# necessary
		num_of_lines++;
	}
}

int see_more()
{
	int c;
	printf("\033[7m more? \033[m");
	while((c=getchar()) != EOF){
		if(c == 'q') return 0;
		else if(c == ' ') return PAGELEN;
		else if(c == '\n') return 1;
	}
	return 0;
}
