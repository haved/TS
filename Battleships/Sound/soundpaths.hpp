#pragma once

#define SOUND(name, path) path
const char *SOUND_PATHS[] = {
#include "soundPaths.txt"
};
#undef SOUND

#define MUSIC(name, path) path
const char *MUSIC_PATHS[] = {
#include "musicPaths.txt"
};
#undef MUSIC
