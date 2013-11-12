CC=g++
CFLAGS=-c -Wall
SOURCES=Routing/Router.cpp Routing/RoutingTable.cpp Links/Link.cpp Main/Main.cpp\
 Flow/Flow.cpp Packet/Packet.cpp Control/Control.cpp Node/Node.cpp\
 Node/Host.cpp InputParser/InputParser.cpp
PLOTSOURCES=gnuplot/make_plot.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=test
INCLUDE=-I Control -I Flow -I Links -I Node -I Routing -I Packet -I Main\
        -I InputParser

all: test

test: $(SOURCES)
	$(CC) $(SOURCES) -o bin/$@ $(INCLUDE) -g

plot_test: $(PLOTSOURCES)
	$(CC) $(PLOTSOURCES) -o bin/$@

