CC=gcc
CFLAGS=-c -Wall 
LDFLAGS= -lncurses
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
