#pragma once
#include <stack>
#include <memory>

#include "SerialIO.hpp"

struct ButtonState {
	struct {
		bool up, down, left, right, action, start;
	} player[2];
};

class Mode;
struct ModeDeleter {
	void operator()(Mode* mode);
};
using ModeUniquePtr = std::unique_ptr<Mode, ModeDeleter>;
using ModeStack = std::stack<ModeUniquePtr>;

class Mode {
private:
	ButtonState buttons = {};
	ButtonState prevButtons = {};
	SerialIO& m_serial;
	bool m_initialized = false;
protected:
	virtual void update_mode(ModeStack& modeStack)=0;
public:
	Mode(SerialIO& io) : m_serial(io) {}
	virtual ~Mode();
	virtual void init()=0;
	void update(ModeStack& modeStack);
};

class MenuMode : public Mode {
public:
	MenuMode(SerialIO& io) : Mode(io) {}
	void init() override;
	void update_mode(ModeStack& mode) override;
};
