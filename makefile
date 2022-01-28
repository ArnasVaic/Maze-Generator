CC = gcc # your compiler
CFLAGS = -Wall -Wextra -Wpedantic -Werror
OUT = maze_gen.dll

.DEFAULT_GOAL := release

debug: maze_generator.c maze_generator.h
	$(CC) -shared -DDEBUG_MAZE -o maze_gen.dll maze_generator.c -I. 

release: maze_generator.c maze_generator.h
	$(CC) -shared -O3 -o $(OUT) maze_generator.c -I.

clean:
	del $(OUT)
.PHONY: clean
