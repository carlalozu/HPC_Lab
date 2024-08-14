CC ?= gcc
CFLAGS = -O3

all: recur_seq recur_omp

recur_seq: recur_seq.o walltime.o
	$(CC)  $(CFLAGS) -o $@ $^

recur_omp: recur_omp.c walltime.o
	$(CC)  $(CFLAGS) -o $@ $^ -fopenmp -lm

%.o: %.c
	$(CC) -c $(CFLAGS) $<

walltime.o: walltime.c walltime.h
	$(CC) -c $<

clean:
	$(RM) *.o recur_seq recur_omp
