#include "Battleship.hpp"

Boat *p1Boats, *p2Boats;
int p1BoatCount, p2BoatCount;
void configureBattleshipsMode(Boat* p1Boats, int p1BoatCount, Boat* p2Boats, int p2BoatCount) {
	::p1Boats = p1Boats;
	::p1BoatCount = p1BoatCount;
	::p2Boats = p2Boats;
	::p2BoatCount = p2BoatCount;
}

