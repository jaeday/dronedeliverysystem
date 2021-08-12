CXX = g++
CXXFLAGS = -std=c++1z -O3 -Wall -Werror -pedantic
SRCFILE = $(shell pwd)/routeplanning/tsp.cpp
EXECUTABLE = route
DEBUG_EXECUTABLE = route_debug
OBJECTS = *.o *.dSYM tsp

all:
	$(CXX) $(CXXFLAGS) $(SRCFILE) -o $(EXECUTABLE)

clean:
	rm $(OBJECTS) $(EXECUTABLE) $(DEBUG_EXECUTABLE) out*.txt $(shell pwd)/routeplanning/tsp

debug:
	$(CXX) -std=c++1z -g3 -Wall -Werror -pedantic $(SRCFILE) -o $(DEBUG_EXECUTABLE)

phony:
	all, clean