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


int dlshStart(
				dlshPrintFuncType printFuncParam,
				dlshGetCharFuncType getCharFuncParam,
				int printInput
			);
void dlshExit(void);



#endif	/* __DLSH_H__ */
