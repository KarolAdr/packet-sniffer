path=..
CC=gcc
CFLAGS=-c -g -pthread -Wall -I$(path)/traffic-visualizer-master/include/
LDFLAGS= -L$(path)/traffic-visualizer-master/build/ -ltrafficvisualizer -lncurses -pthread
SOURCES= main.c visualizer.c $(path)/semaphore/semop.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=visualizer

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c.o: 
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o 
