CC = gcc
CFLAGS = -Wall -Werror -O3 -march=native
LIBS = -lm #-msse4.2

sieve_proto: prototypeSeg.c
	$(CC) $(CFLAGS) -o sieve_proto prototypeSeg.c -fopenmp $(LIBS)

sieve_seg: segmented.c
	$(CC) $(CFLAGS) -o sieve_seg segmented.c -lpthread $(LIBS)

sieve_mt: multiThread.c
	$(CC) $(CFLAGS) -o sieve_mt multiThread.c -lpthread $(LIBS)

sieve_proto_debug: prototypeSeg.c
	$(CC)  -Wall -Werror -march=native -pg -O3  -o sieve_proto_debug prototypeSeg.c -fopenmp $(LIBS)

.PHONY: clean

clean:
	rm -f sieve_proto sieve_seg sieve_mt sieve_proto_debug *.o *.txt *.out
