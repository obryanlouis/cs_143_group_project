CC=g++
CFLAGS=-c -Wall
SOURCES=Routing/Router.cpp Routing/RoutingTable.cpp Links/Link.cpp Main/Main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=test
INCLUDE=-I Links -I Routing

all: test
	
test: $(SOURCES)
	$(CC) $(SOURCES) -o bin/$@ $(INCLUDE)

