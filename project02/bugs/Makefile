CC     ?= gcc
CFLAGS  = -O3

all: omp_bug1 omp_bug2 omp_bug3 omp_bug4 omp_bug5


omp_bug2.o: omp_bug2.c
	$(CC) -c $(CFLAGS) -fopenmp $<

omp_bug2: omp_bug2.o
	$(CC) $(CFLAGS) -fopenmp -o $@ $^

omp_bug1.o: omp_bug1.c
	$(CC) -c $(CFLAGS) -fopenmp $<

omp_bug1: omp_bug1.o
	$(CC) $(CFLAGS) -fopenmp -o $@ $^

omp_bug3.o: omp_bug3.c
	$(CC) -c $(CFLAGS) -fopenmp $<

omp_bug3: omp_bug3.o
	$(CC) $(CFLAGS) -fopenmp -o $@ $^

omp_bug5.o: omp_bug5.c
	$(CC) -c $(CFLAGS) -fopenmp $<

omp_bug5: omp_bug5.o
	$(CC) $(CFLAGS) -fopenmp -o $@ $^

omp_bug4.o: omp_bug4.c
	$(CC) -c $(CFLAGS) -fopenmp $<

omp_bug4: omp_bug4.o
	$(CC) $(CFLAGS) -fopenmp -o $@ $^

.PHONY: clean
clean:
	$(RM) *.o mandel_seq mandel_par
