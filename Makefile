CC=g++
CFLAGS=-c -Wall
SOURCES=Routing/Router.cpp Routing/RoutingTable.cpp Links/Link.cpp Main/Main.cpp Flow/Flow.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=test
INCLUDE=-I Control -I Flow -I Links -I Node -I Routing -I Packet

all: test

test: $(SOURCES)
	$(CC) $(SOURCES) -o bin/$@ $(INCLUDE)

