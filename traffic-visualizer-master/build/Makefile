CC=gcc
EXECUTABLE=monit
LIBRARY=libtrafficvisualizer.so
EXE_CFLAGS=-c -Wall -g -I../include
LIB_CFLAGS=-c -Wall -s -I../include -fPIC
EXE_LDFLAGS=-lncurses -L. -ltrafficvisualizer
LIB_LDFLAGS=-shared -Wl,-soname,$(LIBRARY).1
EXE_SOURCES=../src/main.c
LIB_SOURCES=../src/traffic-visualizer.c
EXE_OBJECTS=$(EXE_SOURCES:.c=.o)
LIB_OBJECTS=$(LIB_SOURCES:.c=.o)

all: $(LIBRARY) $(EXECUTABLE)


$(EXECUTABLE) : $(EXE_OBJECTS)
	$(CC) $(EXE_OBJECTS) $(EXE_LDFLAGS) -o $(EXECUTABLE)

$(LIBRARY) : $(LIB_OBJECTS)
	$(CC) $(LIB_OBJECTS) $(LIB_LDFLAGS) -o $(LIBRARY).1.0.0
	ln -s $(LIBRARY).1.0.0 $(LIBRARY).1
	ln -s $(LIBRARY).1.0.0 $(LIBRARY)

$(EXE_OBJECTS) : $(EXE_SOURCES)
	$(CC) $(EXE_CFLAGS) $< -o $@
	echo "LD_LIBRARY_PATH=. ./$(EXECUTABLE)" > run.sh
	chmod u+x run.sh

$(LIB_OBJECTS) : $(LIB_SOURCES)
	$(CC) $(LIB_CFLAGS) $< -o $@

clean:
	rm -f $(EXE_OBJECTS)
	rm -f $(EXECUTABLE)
	rm -f $(LIB_OBJECTS)
	rm -f $(LIBRARY)
	rm -f $(LIBRARY).1
	rm -f $(LIBRARY).1.0.0
	rm -f run.sh
