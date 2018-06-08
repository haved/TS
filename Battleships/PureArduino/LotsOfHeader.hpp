#pragma once
#include "ArduinoInterface.hpp"
#include "GameLogic.hpp"
#include "Battleship.hpp"

void drawOceanTile(int screen, int x, int y);
void drawWholeOcean(int screen);

void configureShipPlaceMode(bool twoPlayer_p);
void configureBattleshipsMode(Boat* p1Boats, int p1BoatCount, Boat* p2Boats, int p2BoatCount);
