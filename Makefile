CXX = g++
CXXFLAGS = -std=c++1z -O3 -Wall -Werror -pedantic
SRCFILE = $(shell pwd)/routeplanning/tsp.cpp
EXECUTABLE = route
OBJECTS = *.o *.dSYM

all:
	$(CXX) $(CXXFLAGS) $(SRCFILE) -o $(EXECUTABLE)

clean:
	rm $(OBJECTS) $(EXECUTABLE)

phony:
	all, clean