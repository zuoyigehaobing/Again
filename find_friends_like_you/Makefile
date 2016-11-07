CC = gcc
CFLAGS = -Wall -g -std=c99


all: questions binary_tree cate

questions:
	$(CC) $(CFLAGS) questions.c test1.c -o test1

binary_tree:
	$(CC) $(CFLAGS) qtree.c questions.c test2.c -o test2

cate:
	$(CC) $(CFLAGS) qtree.c questions.c categorizer.c -o categorizer
    
clean:  
	rm test1 test2 categorizer
