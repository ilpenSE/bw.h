CC = clang
CFLAGS = -Wall -Wextra -ggdb -fsanitize=undefined,address

main: main.c
	$(CC) $(CFLAGS) -o main main.c
