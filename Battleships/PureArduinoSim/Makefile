CXX=g++
CFLAGS_GAME=-std=c++14 -g
CFLAGS_SIM=`pkg-config --cflags gtk+-3.0 SDL2_mixer` -std=c++14 -g
LFLAGS=`pkg-config --libs gtk+-3.0 SDL2_mixer vorbis ogg flac`
DEPS = ../PureArduino/ArduinoInterface.hpp

#build_sim is where the simulation files are put
#build_game is where the game files are put

SimSourceFiles := $(wildcard ./*.cpp)
SimBuildFiles := $(patsubst ./%.cpp, build_sim/%.o, $(SimSourceFiles))

GameSourceFiles := $(wildcard ../PureArduino/*.cpp)
GameBuildFiles := $(patsubst ../PureArduino/%.cpp, build_game/%.o, $(GameSourceFiles))


all: $(SimBuildFiles) $(GameBuildFiles)
	$(CXX) $^ -o ArduinoSim $(LFLAGS)

build_sim:
	mkdir build_sim

build_game:
	mkdir build_game

build_sim/%.o: %.cpp build_sim $(DEPS)
	$(CXX) $(CFLAGS_SIM) -c -o $@ $<

build_game/%.o: ../PureArduino/%.cpp build_game $(DEPS)
	$(CXX) $(CFLAGS_GAME) -c -o $@ $<

clean:
	rm -r build_sim build_game
	rm ArduinoSim
