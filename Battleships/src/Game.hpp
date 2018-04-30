#pragma once
#include <stack>
#include <memory>

#include "SerialIO.hpp"
#include "Util.hpp"
#include "ArduinoEncoder.hpp"

extern SerialIO* global_serial_ptr;

bool clicked(int framesHeld);

class Mode;
struct ModeDeleter {
	void operator()(Mode* mode);
};
using ModeUniquePtr = std::unique_ptr<Mode, ModeDeleter>;
using ModeStack = std::stack<ModeUniquePtr>;

class Mode {
private:
	bool m_initialized = false;
	long long m_frameCount = 0;
protected:
	virtual void update_mode(ModeStack& modeStack)=0;
public:
	virtual ~Mode();
	virtual void init(){};
	virtual void onFocus(){};
	void update(ModeStack& modeStack);
	long long getFrameCount();
};

class MenuMode : public Mode {
	int m_currentChoice;
public:
	void init() override;
	void onFocus() override;
	void update_mode(ModeStack& mode) override;
};

class PlaceShipsMode : public Mode {
public:
    void onFocus() override;
	void update_mode(ModeStack& mode) override;
};

class GameMode : public Mode {
public:
	void init() override;
	void onFocus() override;
	void update_mode(ModeStack& mode) override;
};

class TransitionMode : public Mode {
	CRGB from;
	CRGB to;
	int frames;
public:
	TransitionMode(CRGB from, CRGB to, int frames);
    void update_mode(ModeStack& mode) override;
};

class InGameMenu : public Mode {
	Player player;
public:
	InGameMenu(Player player) : player(player) {}
	void onFocus() override;
	void update_mode(ModeStack& mode) override;
};
