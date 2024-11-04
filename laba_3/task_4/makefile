CC=gcc

all:
	$(CC) strings.c main.c -o main -ggdb3 
	valgrind --leak-check=full ./main