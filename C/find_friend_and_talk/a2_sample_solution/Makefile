CC = gcc
CFLAGS = -Wall -std=c99 -g

all: questions qtree categorizer

questions: questions.c test1.c
	$(CC) $(CFLAGS) questions.c test1.c -o test1

qtree: qtree.c questions test2.c
	$(CC) $(CFLAGS) qtree.c questions.c test2.c -o test2

categorizer: qtree questions categorizer.c
	$(CC) $(CFLAGS) qtree.c questions.c categorizer.c -o categorizer

clean:  
	rm test1 
