CXX = g++
CXXFLAGS = -std=c++1z -O3
SRCFILES = $(shell pwd)/routeplanning/routeplanning.cpp \
			$(shell pwd)/routeplanning/interface.cpp
EXECUTABLE = route
OBJECTS = *.o *.dSYM

all:
	$(CXX) $(CXXFLAGS) $(SRCFILES) -o $(EXECUTABLE)

clean:
	rm $(OBJECTS) $(EXECUTABLE)

phony:
	all, clean