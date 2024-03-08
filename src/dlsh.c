#include "dlsh.h"


static dlshPrintFuncType dlshPrintFunc = 0;
static dlshExitFuncType dlshExitFunc = 0;


int dlshStart(dlshPrintFuncType dlshPrintFuncParam, dlshExitFuncType dlshExitFuncParam)
{
	if ((0 == dlshPrintFuncParam) || (0 == dlshPrintFuncParam))
	{
		return -1;
	}

	dlshPrintFunc = dlshPrintFuncParam;
	dlshExitFunc = dlshExitFuncParam;

	return 0;
}

