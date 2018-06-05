CXX=g++
CFLAGS=-I. `pkg-config --cflags gtk+-3.0` -std=c++14
DEPS = ../PureArduino/ArduinoInterface.hpp

all: GameLogic.o main.o Util.o
	$(CXX) $^ -o ArduinoSim `pkg-config --libs gtk+-3.0`

GameLogic.o: ../PureArduino/GameLogic.cpp $(DEPS)
	$(CXX) $(CFLAGS) -c -o $@ $<

main.o: main.cpp $(DEPS)
	$(CXX) $(CFLAGS) -c -o $@ $<

Util.o: Util.cpp $(DEPS)
	$(CXX) $(CFLAGS) -c -o $@ $<
