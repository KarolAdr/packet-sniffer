CC=gcc
CFLAGS=-c -pthread -Wall 
LDFLAGS= -pthread 
SOURCES= semop.c main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=semaphore

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c.o: 
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o 
