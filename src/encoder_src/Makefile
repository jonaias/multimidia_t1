#Para escrever comentários ##
############################# Makefile ##########################

CC=gcc
CFLAGS=-Wall -g -ansi

all: encoder

encoder: main.o encode.o
	$(CC) $(CFLAGS) -o ../../bin/encoder main.o encode.o
main.o: main.c
	$(CC) $(CFLAGS) -o main.o -c main.c
encode.o: encode.c encode.h
	$(CC) $(CFLAGS) -o encode.o -c encode.c

clean:
	rm -rf *.o ../../bin/encoder
