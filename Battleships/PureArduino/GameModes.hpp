#pragma once

extern int currMode;
extern int prevMode;
extern int frameCount;
#define MENU_MODE 0
#define HEAVY_TRANSITION_MODE 1

void heavyTransitionTo(int mode, int frames);
void callModeUpdateFunction(int mode, bool changed);
