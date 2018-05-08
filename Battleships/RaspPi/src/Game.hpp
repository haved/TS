#pragma once
#include <vector>
#include <memory>

#include "SerialIO.hpp"
#include "Util.hpp"
#include "ArduinoEncoder.hpp"

extern ButtonState<bool> buttons;
extern ButtonState<bool> prevButtons;
extern ButtonState<int> framesHeld;

class Mode;
struct ModeDeleter {
	void operator()(Mode* mode);
};
using ModeUniquePtr = std::unique_ptr<Mode, ModeDeleter>;
using ModeStack = std::vector<ModeUniquePtr>;

bool clicked(int framesHeld);
void update(ModeStack& stack);

class Mode {
private:
	bool m_initialized = false;
public:
	int frameCount=0;
	virtual ~Mode();
	virtual void onFocus(){};
	virtual void update(ModeStack& modeStack)=0;
};

class MenuMode : public Mode {
	int m_currentChoice;
public:
	void onFocus() override;
	void update(ModeStack& mode) override;
};

class TransitionMode : public Mode {
	int m_frames;
    bool m_viaBlack;
	bool m_started;
public:
	TransitionMode(int frames, bool viaBlack);
	void onFocus() override;
    void update(ModeStack& mode) override;
};

