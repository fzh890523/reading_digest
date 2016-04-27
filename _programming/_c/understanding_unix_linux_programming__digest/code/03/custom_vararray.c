#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct A {
  int len;
  char a[];
};

struct B {
  int len;
  char a[1];
};

int main()
{
  struct A *a = (struct A*)malloc(5 + 1 + sizeof(struct A));
  strcpy(a->a, "12345");
  printf("a.a is %s\n", a->a);

  // 和前面的等效
  struct B *b = (struct B*)malloc(5 + sizeof(struct B));
  strcpy(b->a, "12345");
  printf("b.a is %s\n", b->a);
}

/*
success

output:
a.a is 12345
b.a is 12345
*/
