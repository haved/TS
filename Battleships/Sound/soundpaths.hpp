#pragma once

#define SOUND(name, path) path
const char *soundPaths[] = {
#include "soundPaths.txt"
};

#define MUSIC(name, path) path
const char *musicPaths[] = {
#include "musicPaths.txt"
};
