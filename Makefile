CC = gcc
CFLAGS = -Wall -Werror -O3
LIBS = -lpthread -lm

sieve_seg: segmented.c
	$(CC) $(CFLAGS) -o sieve_seg segmented.c $(LIBS)

sieve_mt: multiThread.c
	$(CC) $(CFLAGS) -o sieve_mt multiThread.c $(LIBS)

sieve_proto: prototypeSeg.c
	$(CC) $(CFLAGS) -o sieve_proto prototypeSeg.c $(LIBS)
