#Para escrever comentários ##
############################# Makefile ##########################

CC=gcc
CFLAGS=-Wall -g -ansi -lm

all: encoder

encoder: main.o encode.o ../util/arraystream.o
	$(CC) $(CFLAGS) -o ../../bin/encoder main.o encode.o ../util/arraystream.o
main.o: main.c
	$(CC) $(CFLAGS) -o main.o -c main.c
encode.o: encode.c encode.h
	$(CC) $(CFLAGS) -o encode.o -c encode.c
../util/arraystream.o: ../util/arraystream.c ../util/arraystream.h
	$(CC) $(CFLAGS) -o ../util/arraystream.o -c ../util/arraystream.c

clean:
	rm -rf *.o ../../bin/encoder
