#pragma once

#define SOUND(name, path) path
const char *SOUND_PATHS[] = {
#include "soundPaths.hpp"
};
#undef SOUND

#define MUSIC(name, path) path
const char *MUSIC_PATHS[] = {
#include "musicPaths.hpp"
};
#undef MUSIC
