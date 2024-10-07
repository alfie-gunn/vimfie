remove_test: run_test
	rm test

run_test: build_test
	./test

build_test:
	gcc -o test test.c ../file_contents.c ../cursor.c -Wall -Werror