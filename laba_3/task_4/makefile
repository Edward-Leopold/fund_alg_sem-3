CC=gcc

all:
	$(CC) strings.c main.c -o main -ggdb3 
	valgrind --leak-check=full --track-origins=yes -s ./main