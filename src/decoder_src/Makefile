#Para escrever comentários ##
############################# Makefile ##########################

CC=gcc
CFLAGS=-Wall -g -ansi -lm

all: encoder

encoder: main.o decode.o ../util/arraystream.o
	$(CC) $(CFLAGS) -o ../../bin/decoder main.o decode.o ../util/arraystream.o
main.o: main.c
	$(CC) $(CFLAGS) -o main.o -c main.c
decode.o: decode.c decode.h
	$(CC) $(CFLAGS) -o decode.o -c decode.c
../util/arraystream.o: ../util/arraystream.c ../util/arraystream.h
	$(CC) $(CFLAGS) -o ../util/arraystream.o -c ../util/arraystream.c
	
clean:
	rm -rf *.o ../../bin/decoder
