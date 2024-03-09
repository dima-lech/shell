#include "dlsh.h"


#ifdef	DLSH_DEBUG
	#define DO_DEBUG(x)		x
#else
	#define DO_DEBUG(x)
#endif


#define BUILT_IN_COMMANDS_NUM	2
#define MAX_STRING_LEN			1024
#define MAX_COMMAND_STRING_LEN	32
#define MAX_COMMANDS			256
#define MAX_ARGV_LEN			256
#define MAX_ARGV_COUNT			32
#define DEFAULT_PROMPT			"[DLSH]> "
#define CHAR_NEWLINE			'\n'
#define CHAR_NEWLINE1			CHAR_NEWLINE
#define CHAR_NEWLINE2			'\r'
#define CHAR_ESCAPE				'\x1b'
#define CHAR_SPACE				' '
#define CHAR_NULL				'\0'
#define CHAR_BACKSPACE			0x7F
#define STR_DELETE				"\033[D \033[D"

typedef struct
{
	dlshCommandFuncType		function;
	char					string[MAX_COMMAND_STRING_LEN];
} DLSH_COMMAND_TYPE;



static dlshPrintFuncType dlshPrintFunc = 0;
static dlshGetCharFuncType dlshGetCharFunc = 0;
static int exitFlag = 0;
static int commandsNum = 0;
static DLSH_COMMAND_TYPE commandsList[MAX_COMMANDS];
static char argvLocal[MAX_ARGV_COUNT][MAX_ARGV_LEN];
static char * argvGlobal[MAX_ARGV_COUNT];
static int argc = 0;



static void dlshHelpCommand(int argc, char * argv[]);
static void dlshExitCommand(int argc, char * argv[]);
static void dlshDoCommand(void);
void printArgDebug(void);
static int strcmp(const char * str1, const char * str2);



#ifdef	DLSH_DEBUG
void printArgDebug(void)
{
	int i;

	dlshPrintFunc("Entered command:\n");
	for (i = 0; i < argc; i++)
	{
		dlshPrintFunc("\t");
		dlshPrintFunc(argvLocal[i]);
		dlshPrintFunc("\n");
	}
}
#endif

static void dlshDoCommand(void)
{
	int i;

	DO_DEBUG(printArgDebug());

	for (i = 0; i < argc; i++)
	{
		argvGlobal[i] = argvLocal[i];
	}

	for (i = 0; i < commandsNum; i++)
	{
		if (strcmp(argvLocal[0], commandsList[i].string))
		{
			commandsList[i].function(argc, argvGlobal);
			return;
		}
	}

	dlshPrintFunc("'");
	dlshPrintFunc(argvLocal[0]);
	dlshPrintFunc("' command not found!\n");
}


static void dlshHelpCommand(__attribute__((unused)) int argc, __attribute__((unused)) char * argv[])
{
	int i;

	if (commandsNum <= BUILT_IN_COMMANDS_NUM)
	{
		dlshPrintFunc("No commands were registered!\n");
		return;
	}

	dlshPrintFunc("Available commands:\n");
	for (i = 0; i < (commandsNum - BUILT_IN_COMMANDS_NUM); i++)
	{
		dlshPrintFunc("\t");
		dlshPrintFunc(commandsList[i].string);
		dlshPrintFunc("\n");
	}
	dlshPrintFunc("\n");
}

static void dlshExitCommand(__attribute__((unused)) int argc, __attribute__((unused)) char * argv[])
{
	dlshPrintFunc("\nExiting shell...\t=^.^=\n\n");
	exitFlag = 1;
}


int dlshStart(dlshPrintFuncType printFuncParam, dlshGetCharFuncType getCharFuncParam, int printInput)
{
	char inputCharStr[2] = " ";
	int argvLen = 0;

	if ((0 == printFuncParam) || (0 == getCharFuncParam))
	{
		return -1;
	}

	dlshPrintFunc = printFuncParam;
	dlshGetCharFunc = getCharFuncParam;

	/* Register built-in 'help' command */
	dlshRegisterCommand("help", dlshHelpCommand);
	/* Register built-in 'exit' command */
	dlshRegisterCommand("exit", dlshExitCommand);

	dlshPrintFunc("\n=== DLSH START ");
	DO_DEBUG(dlshPrintFunc("[DEBUG MODE ON] "));
	dlshPrintFunc("===\n");
	dlshPrintFunc("Type 'help' for list of available commands.\n");
	dlshPrintFunc("Type 'exit' to close shell.\n\n");

	dlshPrintFunc(DEFAULT_PROMPT);

	/**
	 * MAIN LOOP
	 **/
	while (!exitFlag)
	{
		inputCharStr[0] = dlshGetCharFunc();

		if ((CHAR_NEWLINE1 == inputCharStr[0]) || (CHAR_NEWLINE2 == inputCharStr[0]))
		{
			if (printInput)
			{
				inputCharStr[0] = CHAR_NULL;
				dlshPrintFunc("\n");
			}

			if (argvLen > 0)
			{
				argvLocal[argc][argvLen++] = CHAR_NULL;
				/* TODO: error handling */
				argc++;
				argvLen = 0;
			}

			if (argc > 0)
			{
				dlshDoCommand();
			}

			if (exitFlag != 1)
			{
				dlshPrintFunc(DEFAULT_PROMPT);
			}
			argc = 0;
			argvLen = 0;
		}
		else if (CHAR_ESCAPE == inputCharStr[0])
		{
			dlshPrintFunc("\n");
			dlshPrintFunc(DEFAULT_PROMPT);
			argc = 0;
			argvLen = 0;
		}
		else if (CHAR_SPACE == inputCharStr[0])
		{
			argvLocal[argc][argvLen] = CHAR_NULL;

			if (argvLen > 0)
			{
				/* TODO: error handling */
				argc++;
				argvLen = 0;
			}
		}
		else if ((inputCharStr[0] >= 33) && (inputCharStr[0] <= 126))
		{
			argvLocal[argc][argvLen++] = inputCharStr[0];
		}
		else if (CHAR_BACKSPACE == inputCharStr[0])
		{
			if (argvLen > 0)
			{
				argvLen--;
				dlshPrintFunc(STR_DELETE);
			}
			inputCharStr[0] = CHAR_NULL;
		}
		else
		{
			inputCharStr[0] = CHAR_NULL;
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

	if (0 != dlshPrintFunc)
	{
		dlshPrintFunc(str);
	}

	dlshExitCommand(0, 0);
}


int dlshRegisterCommand(char * string, dlshCommandFuncType function)
{
	int i;

	if ((MAX_COMMANDS == commandsNum) || (0 == function))
	{
		return -1;
	}

	for (i = 0; (i < MAX_COMMAND_STRING_LEN) && (string[i] != '\0'); i++)
	{
		commandsList[commandsNum].string[i] = string[i];
	}
	commandsList[commandsNum].function = function;
	commandsNum++;

	return 0;
}



static int strcmp(const char * str1, const char * str2)
{
	int i;

	for (i = 0; i < MAX_STRING_LEN; i++)
	{
		if (str1[i] != str2[i])
		{
			return 0;
		}
		else if (str1[i] == CHAR_NULL)
		{
			return 1;
		}
	}

	return 0;
}



