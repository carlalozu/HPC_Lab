# Some possible configs ########################################################

# Euler GCC config (module load gcc openmpi)
CC = gcc                         # compiler (& linker)
OPT = -Xclang -fopenmp
CFLAGS = -Wall -std=gnu99 -I/opt/homebrew/opt/libomp/include $(OPT) # standard compiler flags
LDLIBS = -lm -L/opt/homebrew/opt/libomp/lib -lomp

################################################################################

targets = first_private
objects = first_private.o

.PHONY: default
default: all

.PHONY: all
all: $(targets)

first_private: first_private.o
	$(CC) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) $<

.PHONY: clean
clean:
	$(RM) $(targets) $(objects)

.PHONY: cleanall
cleanall: clean
	$(RM) $(results)
