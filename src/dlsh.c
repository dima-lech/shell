#include "dlsh.h"


#define DEFAULT_PROMPT		"[DLSH]> "
#define CHAR_NEWLINE		'\n'


static dlshPrintFuncType dlshPrintFunc = 0;
static dlshGetCharFuncType dlshGetCharFunc = 0;
static int exitFlag = 0;


int dlshStart(dlshPrintFuncType printFuncParam, dlshGetCharFuncType getCharFuncParam, int printInput)
{
	char inputCharStr[1] = "";

	if ((0 == printFuncParam) || (0 == getCharFuncParam))
	{
		return -1;
	}

	dlshPrintFunc = printFuncParam;
	dlshGetCharFunc = getCharFuncParam;

	dlshPrintFunc("dlshStart()\n");

	dlshPrintFunc(DEFAULT_PROMPT);

	/**
	 * MAIN LOOP
	 **/
	while (!exitFlag)
	{
		inputCharStr[0] = dlshGetCharFunc();

		if (CHAR_NEWLINE == inputCharStr[0])
		{
			dlshPrintFunc(DEFAULT_PROMPT);			
		}

		if (printInput)
		{
			dlshPrintFunc(inputCharStr);
		}
	}

	return 0;
}


void dlshExit(void)
{
	char str[1] = { CHAR_NEWLINE };

	exitFlag = 1;

	if (0 != dlshPrintFunc)
	{
		dlshPrintFunc(str);
	}
}

