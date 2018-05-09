#pragma once

#include "Game.hpp"

#define GAME_BG CRGB(50, 60, 200)

class PlaceShipsMode : public Mode {
	int m_p1State;
	int m_p2State;
public:
    void onFocus() override;
	void update(ModeStack& mode) override;
};

class GameMode : public Mode {
public:
	GameMode();
	void onFocus() override;
	void update(ModeStack& mode) override;
};

class InGameMenu : public Mode {
	Player player;
public:
	InGameMenu(Player player) : player(player) {}
	void onFocus() override;
	void update(ModeStack& mode) override;
};
