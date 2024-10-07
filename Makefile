run: build_main
	./bin/vimfie

build_main: run_tests
	gcc src/main.c src/cursor.c src/file_contents.c src/vimfie.c -o bin/vimfie

run_tests:
	cd src/tests && make