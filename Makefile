CC = gcc
CFLAGS = -Wall -Werror -O3
LIBS = -lm

sieve_proto: prototypeSeg.c
	$(CC) $(CFLAGS) -o sieve_proto prototypeSeg.c -fopenmp $(LIBS)

sieve_seg: segmented.c
	$(CC) $(CFLAGS) -o sieve_seg segmented.c -lpthread $(LIBS)

sieve_mt: multiThread.c
	$(CC) $(CFLAGS) -o sieve_mt multiThread.c -lpthread $(LIBS)

