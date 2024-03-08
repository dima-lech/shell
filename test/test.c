#include <stdio.h>
#include <stdlib.h>

#include <termios.h>            //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>     		//STDIN_FILENO

#include "dlsh.h"


#define QUIT_CHAR	'q'



void dlshTestPrint(const char* str);
char dlshTestGetChar(void);


void dlshTestPrint(const char* str)
{
	printf("%s", str);
}

char dlshTestGetChar(void)
{
	char inputChar;

	fflush(stdin);
	inputChar = getchar();

	if (QUIT_CHAR == inputChar)
	{
		dlshExit();
	}

	return inputChar;
}


int main (void)
{
    static struct termios oldt, newt;

	printf("\ndlsh test start\n\n");

	/*tcgetattr gets the parameters of the current terminal
    STDIN_FILENO will tell tcgetattr that it should write the settings
    of stdin to oldt*/
    tcgetattr( STDIN_FILENO, &oldt);
    /*now the settings will be copied*/
    newt = oldt;

    /*ICANON normally takes care that one line at a time will be processed
    that means it will return if it sees a "\n" or an EOF or an EOL*/
    newt.c_lflag &= ~(ICANON);          

    /*Those new settings will be set to STDIN
    TCSANOW tells tcsetattr to change attributes immediately. */
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);


    /**
     * START SHELL
     **/
	dlshStart(dlshTestPrint, dlshTestGetChar, 0);

    
    /*restore the old settings*/
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);

	return 0;
}

