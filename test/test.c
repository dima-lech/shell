#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

#include "dlsh.h"



void dlshTestPrint(const char* str);
char dlshTestGetChar(void);
void dlshTestExit(int s);



void dlshTestExit(int s)
{
	dlshExit();
}


void dlshTestPrint(const char* str)
{
	printf("%s", str);
}

char dlshTestGetChar(void)
{
	char inputChar;

	fflush(stdin);
	inputChar = getchar();

	return inputChar;
}


int main (void)
{
    static struct termios oldt, newt;
    struct sigaction sigIntHandler;

	printf("\nDLSH test start\n");
	printf("ctrl+c to exit\n");

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


    sigIntHandler.sa_handler = dlshTestExit;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);


    /**
     * START SHELL
     **/
	dlshStart(dlshTestPrint, dlshTestGetChar, 0);

    
    /*restore the old settings*/
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);

	return 0;
}

