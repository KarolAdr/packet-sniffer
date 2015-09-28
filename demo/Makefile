CC=gcc
CFLAGS=-c -Wall 
LDFLAGS= 
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
