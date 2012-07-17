#include <stdio.h>

void test()
{
	printf("it worked!\n");
}
int main(int argc, char *argv[])
{
    for(int x=0; x<argc; x++)
      printf("%s\n", argv[x]);
    return 0;
}
