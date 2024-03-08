all:
	gcc -Wall -Werror -DDLSH_DEBUG -Isrc -Itest src/dlsh.c test/test.c -o dlsh-test

clean:
	rm -f dlsh-test
