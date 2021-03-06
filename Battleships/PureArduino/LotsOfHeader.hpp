#pragma once
#include "ArduinoInterface.hpp"
#include "GameLogic.hpp"
#include "Battleship.hpp"

//MenuMode.cpp
void handleHoldEscToMenu();

//PlaceShipsMode.cpp
void configureShipPlaceMode(bool twoPlayer_p);

//BattleshipsMode.cpp
void configureBattleshipsMode(Boat* p1Boats, int p1BoatCount, Boat* p2Boats, int p2BoatCount, bool p2AI);

//OceanDrawer.cpp
void drawOceanTile(int screen, int x, int y, int frame);
void drawWholeOcean(int screen, int frame);

//TetrisMode.cpp
void configureTetrisMode(bool player2);

//PongMode.cpp
void configurePongMode(bool player2);

//CometMode.cpp
void configureCometMode(bool player2);
