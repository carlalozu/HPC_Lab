# Some possible configs ########################################################

# Euler GCC config (module load gcc openmpi)
CC = gcc                         # compiler (& linker)
OPT = -fopenmp
CFLAGS = -O2 $(OPT) # standard compiler flags
LDLIBS = -lm

################################################################################

targets = walltime
objects = walltime.o pi-serial.o pi-critical.o pi-reduction.o

.PHONY: default
default: all

.PHONY: all
all: $(targets)

walltime: walltime.o pi-serial.o pi-critical.o pi-reduction.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) $<

.PHONY: clean
clean:
	$(RM) $(targets) $(objects)

.PHONY: cleanall
cleanall: clean
	$(RM) $(results)
