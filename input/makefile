CC = g++

CFLAGS = -c -Wall
LFLAGS = -o

all: read_input

read_input: read_input.cpp pugixml.o
	$(CC) read_input.cpp $(LFLAGS) run

pugixml.o: pugixml.hpp pugixml.cpp
	$(CC) $(CFLAGS) pugixml.cpp
clean:
	rm -rf *.o *.~ run

