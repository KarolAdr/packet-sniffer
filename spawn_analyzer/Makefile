CC=gcc
CFLAGS=-c -Wall 
LDFLAGS= 
SOURCES= main.c spawner.c 
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=spawn_analyzer

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c.o: 
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o 
