#Para escrever comentários ##
############################# Makefile ##########################

CC=gcc
CFLAGS=-Wall -g -ansi

all: encoder

encoder: main.o decode.o
	$(CC) $(CFLAGS) -o ../../bin/decoder main.o decode.o
main.o: main.c
	$(CC) $(CFLAGS) -o main.o -c main.c
decode.o: decode.c decode.h
	$(CC) $(CFLAGS) -o decode.o -c decode.c

clean:
	rm -rf *.o ../../bin/decoder
