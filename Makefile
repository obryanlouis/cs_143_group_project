CC=g++
CFLAGS=-c -Wall
SOURCES=Routing/Router.cpp Routing/RoutingTable.cpp Links/Link.cpp Main/Main.cpp
PLOTSOURCES=gnuplot/make_plot.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=test
INCLUDE=-I Links -I Routing

all: test
	
test: $(SOURCES)
	$(CC) $(SOURCES) -o bin/$@ $(INCLUDE)

plot_test: $(PLOTSOURCES)
	$(CC) $(PLOTSOURCES) -o bin/$@

