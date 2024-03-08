#include "dlsh.h"


#ifdef	DLSH_DEBUG
	#define DO_DEBUG(x)		x
#else
	#define DO_DEBUG(x)
#endif


#define MAX_STRING_LEN		256
#define DEFAULT_PROMPT		"[DLSH]> "
#define CHAR_NEWLINE		'\n'
#define CHAR_ESCAPE			'\x1b'


static dlshPrintFuncType dlshPrintFunc = 0;
static dlshGetCharFuncType dlshGetCharFunc = 0;
static int exitFlag = 0;
static char inputString[MAX_STRING_LEN];



static void dlshDoCommand(void)
{
	DO_DEBUG(dlshPrintFunc("entered command: \""));
	DO_DEBUG(dlshPrintFunc(inputString));
	DO_DEBUG(dlshPrintFunc("\"\n"));
}


int dlshStart(dlshPrintFuncType printFuncParam, dlshGetCharFuncType getCharFuncParam, int printInput)
{
	char inputCharStr[1] = "";
	int strLen = 0;

	if ((0 == printFuncParam) || (0 == getCharFuncParam))
	{
		return -1;
	}

	dlshPrintFunc = printFuncParam;
	dlshGetCharFunc = getCharFuncParam;

	dlshPrintFunc("\n=== DLSH START ");
	DO_DEBUG(dlshPrintFunc("[DEBUG MODE ON] "));
	dlshPrintFunc("===\n\n");

	dlshPrintFunc(DEFAULT_PROMPT);

	/**
	 * MAIN LOOP
	 **/
	while (!exitFlag)
	{
		inputCharStr[0] = dlshGetCharFunc();

		if (CHAR_NEWLINE == inputCharStr[0])
		{
			if (strLen > 0)
			{
				dlshDoCommand();
			}

			dlshPrintFunc(DEFAULT_PROMPT);
			strLen = 0;
		}
		else if (CHAR_ESCAPE == inputCharStr[0])
		{
			dlshPrintFunc("\n");
			dlshPrintFunc(DEFAULT_PROMPT);
			strLen = 0;
		}
		else
		{
			inputString[strLen++] = inputCharStr[0];
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

