CXX=g++
CFLAGS=`pkg-config --cflags SDL2_mixer` -std=c++14 -g -I ../Sound
DEPS =

SourceFiles := $(wildcard ./*.cpp)
BuildFiles := $(patsubst ./%.cpp, build/%.o, $(SourceFiles))

all: $(BuildFiles)
	$(CXX) $^ -o SoundFromSerial `pkg-config --libs SDL2_mixer`

build:
	mkdir build

build/%.o: %.cpp build $(DEPS)
	$(CXX) $(CFLAGS) -c -o $@ $<

clean:
	rm -r build
	rm SoundFromSerial
