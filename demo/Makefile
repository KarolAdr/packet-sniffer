CC=gcc
CFLAGS=-c -Wall #-I/home/karoladr/trafficgenerator/traffic-visualizer-master/include/
LDFLAGS= #-L/home/karoladr/trafficgenerator/traffic-visualizer-master/build/ -ltrafficvisualizer -lncurses
SOURCES= main.c 
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=demo

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c.o: 
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o 
