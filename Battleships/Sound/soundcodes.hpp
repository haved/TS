#pragma once

#define SOUND(name, path) SOUND_##name
enum SoundCode:char {
#include "soundPaths.hpp"
				,SOUND_COUNT
};
#undef SOUND

#define MUSIC(name, path) MUSIC_##name
enum MusicCode:char {
#include "musicPaths.hpp"
				,MUSIC_COUNT
};
#undef MUSIC
