all:
	gcc -Wall -Werror -Isrc -Itest src/dlsh.c test/test.c -o dlsh-test

clean:
	rm -f dlsh-test
