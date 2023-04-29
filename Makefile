
CXXFLAGS = $(OPTIMIZE) -std=c++11 -I. -MMD

OPTIMIZE = -g

CLASSIC_ENGINE_SRCS = ScreenEngine.cpp SoundEngine.cpp \
	DatabaseFactory.cpp DatabaseImpl.cpp \
	room.cpp title_screen.cpp title_music.cpp prompts.cpp flashing.cpp

SRCS = main.cpp SDLBackend.cpp VDP.cpp DCSG.cpp Timer.cpp \
	StdioFileBackend.cpp \
	init.cpp input.cpp loadsave.cpp new_game.cpp core.cpp \
	$(addprefix classic/,$(CLASSIC_ENGINE_SRCS))

OBJS = $(addprefix build/,$(SRCS:.cpp=.o))
DEPS = $(OBJS:.o=.d)

tunnels : $(OBJS)
	$(CXX) -o $@ $^ -lSDL2

$(OBJS) : build/%.o : %.cpp
	@mkdir -p "$(@D)"
	$(COMPILE.cc) $(OUTPUT_OPTION) $<

-include $(DEPS)

clean :
	-rm -f $(OBJS) $(DEPS) tunnels
