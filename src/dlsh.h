#ifndef		__DLSH_H__
#define		__DLSH_H__


/**
 * Prints a single string
 **/
typedef void (*dlshPrintFuncType)(const char *);

/**
 * Returns a single character input
 **/
typedef char (*dlshGetCharFuncType)(void);

/**
 * Command handler
 **/
typedef char (*dlshCommandFuncType)(int argc, char * argv[]);


int dlshStart(
				dlshPrintFuncType printFuncParam,
				dlshGetCharFuncType getCharFuncParam,
				int printInput
			);
void dlshExit(void);
int dlshRegisterCommand(char * string, dlshCommandFuncType function);


#endif	/* __DLSH_H__ */
