#pragma once
#include <vector>
#include <memory>

#include "SerialIO.hpp"
#include "Util.hpp"
#include "ArduinoEncoder.hpp"

bool clicked(int framesHeld);

class Mode;
struct ModeDeleter {
	void operator()(Mode* mode);
};
using ModeUniquePtr = std::unique_ptr<Mode, ModeDeleter>;
using ModeStack = std::vector<ModeUniquePtr>;

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
	int m_p1State;
	int m_p2State;
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
	int m_frames;
    bool m_viaBlack;
	bool m_started;
public:
	TransitionMode(int frames, bool viaBlack);
	void onFocus() override;
    void update_mode(ModeStack& mode) override;
};

class InGameMenu : public Mode {
	Player player;
public:
	InGameMenu(Player player) : player(player) {}
	void onFocus() override;
	void update_mode(ModeStack& mode) override;
};
