#pragma once
#include <stack>
#include <memory>

#include "SerialIO.hpp"

#define WIDTH 10
#define HEIGHT 10

#define BUTTON_COUNT 12
#define BUTTON_LEFT 0
#define BUTTON_RIGHT 1
#define BUTTON_UP 2
#define BUTTON_DOWN 3
#define BUTTON_ACTION 4
#define BUTTON_START 5
#define PLAYER_2_BUTTONS_OFFSET 6

extern SerialIO* global_serial_ptr;

template<typename T>
struct ButtonState {
	T raw[BUTTON_COUNT];
	auto one() { return &raw[0]; }
	auto two() { return &raw[PLAYER_2_BUTTONS_OFFSET]; }
};
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
	virtual void init()=0;
	void update(ModeStack& modeStack);
	long long getFrameCount();
};

class MenuMode : public Mode {
	int m_currentChoice;
public:
	void init() override;
	void update_mode(ModeStack& mode) override;
};
