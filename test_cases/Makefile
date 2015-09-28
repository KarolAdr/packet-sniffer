CC=gcc
CFLAGS=-c -Wall #-I/home/karoladr/trafficgenerator/traffic-visualizer-master/include/
LDFLAGS= -lncurses#-L/home/karoladr/trafficgenerator/traffic-visualizer-master/build/ -ltrafficvisualizer -lncurses
SOURCES= main.c test.c 
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=menu

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c.o: 
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o 
