CC= g++
SOURCES= gusanos.cpp keyboard.cpp console.cpp bindings.cpp
INCLUDES= keyboard.h console.h bindings.h
CFLAGS= -O3 -mwindows
LIBS= -lfblend -l2xsai -lldpng -lpng -lz -lalleg -lzoidcom_mw
DEFINES= -DAA2XSAI -DWINDOWS -DAAFBLEND
BIN= ../gusanos.exe
OBJECTS= gusanos.o keyboard.o console.o bindings.o

$(BIN): $(OBJECTS)
	$(CC) -o $(BIN) $(OBJECTS) $(CFLAGS) $(LIBS)

.cpp.o:
	$(CC) -c $< $(CFLAGS) $(DEFINES)

clean:
	del $(OBJECTS)
