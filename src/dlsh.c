#include "dlsh.h"


static dlshPrintFuncType dlshPrintFunc = 0;
static int exitFlag = 0;


int dlshStart(dlshPrintFuncType dlshPrintFuncParam)
{
	if (0 == dlshPrintFuncParam)
	{
		return -1;
	}

	dlshPrintFunc = dlshPrintFuncParam;

	dlshPrintFunc("dlshStart()\n");

	return 0;
}


void dlshExit(void)
{
	exitFlag = 1;
}

