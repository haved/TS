CXX=g++
CFLAGS=`pkg-config --cflags SDL2_mixer` -std=c++14 -g -I ../Sound
LFLAGS=`pkg-config --libs SDL2_mixer vorbis ogg flac`
DEPS =

SourceFiles := $(wildcard ./*.cpp)
BuildFiles := $(patsubst ./%.cpp, build/%.o, $(SourceFiles))

all: $(BuildFiles)
	$(CXX) $^ -o SoundFromSerial $(LFLAGS)

build:
	mkdir build

build/%.o: %.cpp build $(DEPS)
	$(CXX) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf build
	rm -rf SoundFromSerial
