#pragma once
#include <stack>
#include <memory>

#include "SerialIO.hpp"

#define WIDTH 10
#define HEIGHT 10

template<typename T>
struct ButtonState {
	struct {
	    T up, down, left, right, action, start;
	} player[2];
	auto one() { return player[0]; }
	auto two() { return player[1]; }
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
	ButtonState<bool> buttons = {};
	ButtonState<bool> prevButtons = {};
	ButtonState<int> framesHeld = {};
	SerialIO& m_serial;
protected:
	virtual void update_mode(ModeStack& modeStack)=0;
public:
	Mode(SerialIO& io) : m_serial(io) {}
	virtual ~Mode();
	virtual void init()=0;
	void update(ModeStack& modeStack);
	long long getFrameCount();
};

class MenuMode : public Mode {
	int m_currentChoice;
public:
	MenuMode(SerialIO& io) : Mode(io) {}
	void init() override;
	void update_mode(ModeStack& mode) override;
};
