#ifndef		__DLSH_H__
#define		__DLSH_H__


/**
 * Prints a single string
 **/
typedef void (*dlshPrintFuncType)(const char *);


int dlshStart(dlshPrintFuncType dlshPrintFuncParam);
void dlshExit(void);



#endif	/* __DLSH_H__ */
