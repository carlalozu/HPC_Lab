CXX     ?= g++
override CXXFLAGS += -O3 -std=c++11

SOURCES = walltime.cpp stats.cpp data.cpp operators.cpp linalg.cpp main.cpp
HEADERS = walltime.h stats.h data.h operators.h linalg.h
OBJ     = walltime.o stats.o data.o operators.o linalg.o

all: main

walltime.o: walltime.cpp walltime.h
	$(CXX) $(CXXFLAGS) -c $<

stats.o: stats.cpp stats.h
	$(CXX) $(CXXFLAGS) -c $<

data.o: data.cpp data.h
	$(CXX) $(CXXFLAGS) -c $<

operators.o: operators.cpp operators.h
	$(CXX) $(CXXFLAGS) -c $<

linalg.o: linalg.cpp linalg.h
	$(CXX) $(CXXFLAGS) -c $<

main.o: main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $<

main: $(OBJ) main.o
	$(CXX) $(CXXFLAGS) $(OBJ) main.o -o $@

.PHONY: clean
clean:
	$(RM) main $(OBJ) output.*
