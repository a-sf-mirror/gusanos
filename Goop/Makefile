CC= g++
SOURCES= gusanos.cpp keyboard.cpp gconsole.cpp keys.cpp
INCLUDES= keyboard.h gconsole.h keys.h
CFLAGS= -O3 -mwindows
LIBS= -lfblend -l2xsai -lldpng -lpng -lz -lalleg -lzoidcom_mw -lgcon
DEFINES= -DAA2XSAI -DWINDOWS -DAAFBLEND
BIN= ../gusanos.exe
OBJECTS= gusanos.o keyboard.o gconsole.o keys.o

$(BIN): $(OBJECTS)
	$(CC) -o $(BIN) $(OBJECTS) $(CFLAGS) $(LIBS)

.cpp.o:
	$(CC) -c $< $(CFLAGS) $(DEFINES)

clean:
	del $(OBJECTS)
