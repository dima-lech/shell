all:
	gcc -Wall -Werror -Isrc -Itest src/dlsh.c test/test.c -o dlsh-test
	gcc -Wall -Werror -Isrc -Itest -DDLSH_USE_STATIC_FUNC -DDLSH_PRINT_FUNC=dlshTestPrint -DDLSH_GETCHAR_FUNC=dlshTestGetChar src/dlsh.c test/test.c -o dlsh-test-static

clean:
	rm -f dlsh-test
	rm -f dlsh-test-static
