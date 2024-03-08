#ifndef		__DLSH_H__
#define		__DLSH_H__


typedef void (*dlshPrintFuncType)(const char *);
typedef void (*dlshExitFuncType)(void);


int dlshStart(dlshPrintFuncType dlshPrintFuncParam, dlshExitFuncType dlshExitFuncParam);



#endif	/* __DLSH_H__ */
