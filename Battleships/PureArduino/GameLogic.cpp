#include "ArduinoInterface.hpp"

ButtonState<bool> buttons = {};
ButtonState<bool> prevButtons = {};
ButtonState<int> framesHeld = {};

bool clicked(int framesHeld) {
  if(framesHeld == 1)
    return true;
  if(framesHeld > 20)
    return (framesHeld-20)%6 == 0;
  return false;
}

#define MENU_MODE 0
int currMode = MENU_MODE;
int prevMode = -1;
int frameCount = 0;
void callModeUpdateFunction(int mode, bool changed);
void changeToTransitionMode(int targetMode, int frames);

#define MENU_BG CRGB(80, 80, 80)
#define MENU_OPTION_BG CRGB(80, 200, 80)
#define MENU_OPTION_COLOR_1 CRGB(255, 0, 0)
#define MENU_OPTION_COLOR_2 CRGB(255, 255, 90)

#define MENU_OPTIONS_Y_OFFSET 1
#define MENU_CHOICE_COUNT 6
void updateMenuMode(bool first) {
  if(first) {
    fillAllScreens(MENU_BG);
    fillRect(PLAYER1 + DEF, 0, MENU_OPTIONS_Y_OFFSET, 1, MENU_OPTIONS_Y_OFFSET + MENU_CHOICE_COUNT, MENU_OPTION_BG);
  }

  CRGB color = interpolate(MENU_OPTION_COLOR_1, MENU_OPTION_COLOR_2, (sin(frameCount)+1)/2);
  static int i = 0;
  setTile(PLAYER1 + DEF, i%WIDTH, i/WIDTH, color);
  i++;
  i%=100;

  if(clicked(framesHeld.one()[BUTTON_A]))
    changeToTransitionMode(MENU_MODE, 60);
}

#define TRANSITION_MODE 1
int transitionTarget = 0;
void changeToTransitionMode(int targetMode, int frames) {
  currMode = TRANSITION_MODE;
  for(int i = 0; i < 4; i++)
    startTransition(i, frames);
  callModeUpdateFunction(targetMode, true);
}

void updateTransitionMode(bool first) {
  setTile(PLAYER1+DEF, 4, 4, CRGB::Black);
  if(anyTransitionRunning())
    return;

  currMode = prevMode = transitionTarget;
}

void callModeUpdateFunction(int mode, bool changed) {
  switch(mode) {
    case MENU_MODE: updateMenuMode(changed); break;
    case TRANSITION_MODE: updateTransitionMode(changed); break;
    default: break;
  }
}

void loop() {
	prevButtons = buttons;
  getButtonStates(buttons);
  for(int i = 0; i < BUTTON_COUNT; i++) {
    auto& fH = framesHeld.raw[i];
    fH = buttons.raw[i] ? fH+1 : 0;
  }

  bool changed = prevMode != currMode;
  prevMode = currMode;
  if(changed)
    frameCount = 0;

  callModeUpdateFunction(currMode, changed);
  updateScreens();

  frameCount++;
}
