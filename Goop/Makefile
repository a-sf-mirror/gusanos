CC= g++
SOURCES= gusanos.cpp keyboard.cpp console.cpp bindings.cpp variables.cpp text.cpp consoleitem.cpp \
	command.cpp gconsole.cpp keys.cpp
INCLUDES= keyboard.h console.h bindings.h variables.h text.h consoleitem.h command.h gconsole.h keys.h
CFLAGS= -O3 -mwindows
LIBS= -lfblend -l2xsai -lldpng -lpng -lz -lalleg -lzoidcom_mw
DEFINES= -DAA2XSAI -DWINDOWS -DAAFBLEND
BIN= ../gusanos.exe
OBJECTS= gusanos.o keyboard.o console.o bindings.o variables.o text.o consoleitem.o command.o gconsole.o keys.o

$(BIN): $(OBJECTS)
	$(CC) -o $(BIN) $(OBJECTS) $(CFLAGS) $(LIBS)

.cpp.o:
	$(CC) -c $< $(CFLAGS) $(DEFINES)

clean:
	del $(OBJECTS)
