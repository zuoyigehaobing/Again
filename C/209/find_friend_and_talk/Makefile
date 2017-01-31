CC = gcc
CFLAGS = -Wall -std=c99 -g

all: categorizer

categorizer: qtree.c questions.c utils.c categorizer.c
	$(CC) $(CFLAGS) qtree.c questions.c categorizer.c utils.c -o categorizer

clean:  
	rm categorizer
