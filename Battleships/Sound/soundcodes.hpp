#pragma once

#define SOUND(name, path) SOUND_##name
enum SoundCode {
#include "soundPaths.txt"
				,SOUND_COUNT
};

#define MUSIC(name, path) MUSIC_##name
enum MusicCode {
#include "musicPaths.txt"
				,MUSIC_COUNT
};