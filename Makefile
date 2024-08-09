C_FILES=src/dlsh.c test/test.c test/mem.c

all:
	gcc -Wall -Werror -Isrc -Itest $(C_FILES) -o dlsh-test
	gcc -Wall -Werror -Isrc -Itest -DDLSH_USE_STATIC_FUNC -DDLSH_PRINT_FUNC=dlshTestPrint -DDLSH_GETCHAR_FUNC=dlshTestGetChar $(C_FILES) -o dlsh-test-static

clean:
	rm -f dlsh-test
	rm -f dlsh-test-static
