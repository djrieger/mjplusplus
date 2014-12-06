#include <stdio.h>

// Compiled using "gcc -S -arch x86_64 print.c"

int _COut_Mprintln(void *_, int num)
{
	printf("%d\n", num);
	return 0;
}
