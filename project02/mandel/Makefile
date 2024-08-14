CC     ?= gcc
CFLAGS  = -O3
LDFLAGS = -O3 -lpng

all: mandel_seq mandel_par


pngwriter.o: pngwriter.c pngwriter.h
	$(CC) -c $(CFLAGS) $<

walltime.o: walltime.c walltime.h
	$(CC) -c $(CFLAGS) $<

mandel_seq.o: mandel_seq.c consts.h pngwriter.h walltime.h
	$(CC) -c $(CFLAGS) $<

mandel_par.o: mandel_par.c consts.h pngwriter.h walltime.h
	$(CC) -c $(CFLAGS) -fopenmp $<

mandel_seq: mandel_seq.o pngwriter.o walltime.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

mandel_par: mandel_par.o pngwriter.o walltime.o
	$(CC) $(CFLAGS) -fopenmp -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	$(RM) *.o mandel_seq mandel_par
