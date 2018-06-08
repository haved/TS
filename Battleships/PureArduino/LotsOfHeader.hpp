#pragma once
#include "ArduinoInterface.hpp"
#include "GameLogic.hpp"
#include "Battleship.hpp"

//MenuMode.cpp
void handleHoldEscToMenu();

//PlaceShipsMode.cpp
void configureShipPlaceMode(bool twoPlayer_p);

//BattleshipsMode.cpp
void configureBattleshipsMode(Boat* p1Boats, int p1BoatCount, Boat* p2Boats, int p2BoatCount);

//OceanDrawer.cpp
void drawOceanTile(int screen, int x, int y, int frame);
void drawWholeOcean(int screen, int frame);
