#include <stdio.h>
#include "dlsh.h"


void dlshTestPrint(const char* str);


void dlshTestPrint(const char* str)
{
	printf("DLSH:\t%s", str);
}




int main (void)
{
	printf("\ndlsh test start\n\n");

	dlshStart(dlshTestPrint);

	return 0;
}

