CC= g++
SOURCES= gusanos.cpp keyboard.cpp gconsole.cpp keys.cpp resource_list.cpp sprite.cpp \
	menu_window.cpp font.cpp vec.cpp level.cpp game.cpp material.cpp viewport.cpp \
	base_object.cpp worm.cpp part_type.cpp particle.cpp \
	base_action.cpp part_events.cpp game_actions.cpp parser.cpp sound.cpp gfx.cpp \
	sfx.cpp base_animator.cpp animators.cpp distortion.cpp player.cpp player_input.cpp \
	base_player.cpp player_options.cpp
CFLAGS= -O3 -mwindows
LIBS= -lldpng -lpng -lz -lalleg -lgcon -lfmod
DEFINES= -DAA2XSAI -DWINDOWS -DAAFBLEND
BIN= ../gusanos.exe
OBJECTS= gusanos.o keyboard.o gconsole.o keys.o resource_list.o sprite.o \
	menu_window.o font.o vec.o level.o game.o material.o viewport.o base_object.o \
	worm.o part_type.o particle.o base_action.o part_events.o \
	game_actions.o parser.o sound.o gfx.o sfx.o base_animator.o animators.o distortion.o \
	player.o player_input.o base_player.o player_options.o

$(BIN): $(OBJECTS)
	$(CC) -o $(BIN) $(OBJECTS) $(CFLAGS) $(LIBS)

.cpp.o:
	$(CC) -c $< $(CFLAGS) $(DEFINES)

clean:
	del $(OBJECTS)
