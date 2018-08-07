#pragma once

#include "soundcodes.hpp"

void playSoundEffect(SoundCode soundCode);
void pauseSoundEffects();
void resumeSoundEffects();
void stopSoundEffects();
void loopMusic(MusicCode musicCode);
void pauseMusic();
void resumeMusic();
void fadeOutMusic(char millis);
