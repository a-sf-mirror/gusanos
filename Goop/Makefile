CC= g++
SOURCES= gusanos.cpp keyboard.cpp gconsole.cpp keys.cpp resource_list.cpp sprite.cpp \
	menu_window.cpp font.cpp vec.cpp level.cpp game.cpp material.cpp viewport.cpp \
	base_object.cpp test_particle.cpp
CFLAGS= -O3 -mwindows
LIBS= -lfblend -l2xsai -lldpng -lpng -lz -lalleg -lzoidcom_mw -lgcon
DEFINES= -DAA2XSAI -DWINDOWS -DAAFBLEND
BIN= ../gusanos.exe
OBJECTS= gusanos.o keyboard.o gconsole.o keys.o resource_list.o sprite.o \
	menu_window.o font.o vec.o level.o game.o material.o viewport.o base_object.o \
	test_particle.o

$(BIN): $(OBJECTS)
	$(CC) -o $(BIN) $(OBJECTS) $(CFLAGS) $(LIBS)

.cpp.o:
	$(CC) -c $< $(CFLAGS) $(DEFINES)

clean:
	del $(OBJECTS)
