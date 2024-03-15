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
static char commandLine[MAX_STRING_LEN];


static void dlshHelpCommand(int argc, char * argv[]);
static void dlshExitCommand(int argc, char * argv[]);
static void dlshDoCommand(void);
static int strcmp(const char * str1, const char * str2);



static void dlshDoCommand(void)
{
	int i;
	int argc = 0;
	int argvLen = 0;

	DO_DEBUG(dlshPrintFunc("Command line string: \""));
	DO_DEBUG(dlshPrintFunc(commandLine));
	DO_DEBUG(dlshPrintFunc("\"\r\n"));

	/* Parse command line */
	for (i = 0; (i < MAX_STRING_LEN) && (commandLine[i] != CHAR_NULL); i++)
	{
		if (commandLine[i] == CHAR_SPACE)
		{
			if (argvLen > 0)
			{
				argvLocal[argc][argvLen] = CHAR_NULL;
				argc++;
				argvLen = 0;
			}
		}
		else
		{
			argvLocal[argc][argvLen++] = commandLine[i];
		}
	}

	/* Count last argument */
	if (argvLen > 0)
	{
		argvLocal[argc][argvLen] = CHAR_NULL;
		argc++;
	}

	if (argc == 0)
	{
		return;
	}

	/* Populate argv pointers */
	for (i = 0; i < argc; i++)
	{
		argvGlobal[i] = argvLocal[i];
	}

	/* Search for command */
	for (i = 0; i < commandsNum; i++)
	{
		if (strcmp(argvLocal[0], commandsList[i].string))
		{
			/* Perform command */
			commandsList[i].function(argc, argvGlobal);
			return;
		}
	}

	/* Command not found */
	dlshPrintFunc("'");
	dlshPrintFunc(argvLocal[0]);
	dlshPrintFunc("' command not found!\r\n");
}


static void dlshHelpCommand(__attribute__((unused)) int argc, __attribute__((unused)) char * argv[])
{
	int i;

	if (commandsNum <= BUILT_IN_COMMANDS_NUM)
	{
		dlshPrintFunc("No commands were registered!\r\n");
		return;
	}

	dlshPrintFunc("Available commands:\r\n");
	for (i = 0; i < (commandsNum - BUILT_IN_COMMANDS_NUM); i++)
	{
		dlshPrintFunc("\t");
		dlshPrintFunc(commandsList[i].string);
		dlshPrintFunc("\r\n");
	}
	dlshPrintFunc("\r\n");
}

static void dlshExitCommand(__attribute__((unused)) int argc, __attribute__((unused)) char * argv[])
{
	dlshPrintFunc("\r\nExiting shell...\t=^.^=\r\n\n");
	exitFlag = 1;
}


int dlshStart(dlshPrintFuncType printFuncParam, dlshGetCharFuncType getCharFuncParam, int printInput)
{
	char inputCharStr[2] = " ";
	int commandLineLen = 0;

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

	dlshPrintFunc("\r\n=== DLSH START ");
	DO_DEBUG(dlshPrintFunc("[DEBUG MODE ON] "));
	dlshPrintFunc("===\r\n");
	dlshPrintFunc("Type 'help' for list of available commands.\r\n");
	dlshPrintFunc("Type 'exit' to close shell.\r\n\n");

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
				dlshPrintFunc("\r\n");
			}

			/* Terminate command line string */
			commandLine[commandLineLen] = CHAR_NULL;

			if (commandLineLen > 0)
			{
				dlshDoCommand();
			}

			if (exitFlag != 1)
			{
				dlshPrintFunc(DEFAULT_PROMPT);
			}

			commandLineLen = 0;
		}
		else if (CHAR_ESCAPE == inputCharStr[0])
		{
			dlshPrintFunc("\r\n");
			dlshPrintFunc(DEFAULT_PROMPT);
			commandLineLen = 0;
		}
		else if ((inputCharStr[0] >= 32) && (inputCharStr[0] <= 126))
		{
			commandLine[commandLineLen++] = inputCharStr[0];
		}
		else if (CHAR_BACKSPACE == inputCharStr[0])
		{
			if (commandLineLen > 0)
			{
				commandLineLen--;
				dlshPrintFunc(STR_DELETE);
			}
			if (!printInput)
			{
				dlshPrintFunc(STR_DELETE);
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
	char str[2] = { CHAR_NEWLINE2, CHAR_NEWLINE1 };

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



