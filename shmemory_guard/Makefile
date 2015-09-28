CC=gcc
CFLAGS=-c -Wall 
LDFLAGS= 
SOURCES= main.c shmem.c 
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=memory_guard

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c.o: 
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o 
