#include "Game.hpp"
#include "ArduinoEncoder.hpp"

void ModeDeleter::operator ()(Mode* mode) {
	delete mode;
}

void Mode::update(ModeStack& modeStack) {
	if(!m_initialized) {
		init();
		m_initialized = true;
	}
	prevButtons = buttons;
	updateButtonState(m_serial, buttons);
	update_mode(modeStack);
}

