#include "AudioSystem.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdexcept>
#include <iostream>
#include <map>
#include <cstring>

//Plagiarism from: https://gist.github.com/armornick/3497064

#define ERROR(s) do{std::cerr << "AudioSystem:" << __LINE__ << ": " << s << std::endl; return;}while(false)

void startAudioSystem() {
	if(SDL_Init(SDL_INIT_AUDIO) != 0)
		ERROR("Failed to init SDL for audio" << SDL_GetError());
	if(Mix_OpenAudio(22050, AUDIO_S16SYS, 1, 1024) == -1)
		ERROR("Failed to Mix_OpenAudio" << SDL_GetError());
}

std::map<const char*, Mix_Chunk*> waves;

void playSound(const char* PATH) {
	Mix_Chunk* wave;
	auto find = waves.find(PATH);
	if(find == waves.end()) {
		wave = Mix_LoadWAV(PATH);
		if(wave == nullptr)
			ERROR("Failed loading wave: '" << PATH << "': " << Mix_GetError());
		waves[PATH] = wave;
	} else
		wave = find->second;

	//-1 means first free channel
	int result = Mix_PlayChannel(-1, wave, 0); //0 num loops
	if(result == -1) { //Maybe all channels are full?
		stopSoundEffects(); //So we stop them all
		result = Mix_PlayChannel(-1, wave, 0); //try again
	}
	if(result == -1)
		ERROR("Mix_PlayChannel failed: '" << PATH << "': " << Mix_GetError());
}

std::map<const char*, Mix_Music*> musics;

const char* currentMusic;
void loopMusic(const char* PATH) {

	if(currentMusic && strcmp(PATH, currentMusic) == 0) {
		resumeMusic();
		return;
	}

	Mix_Music* music;
	auto find = musics.find(PATH);
	if(find == musics.end()) {
	    music = Mix_LoadMUS(PATH);
		if(music == nullptr)
			ERROR("Failed loading music: '" << PATH << "': " << Mix_GetError());
		musics[PATH] = music;
	} else
		music = find->second;

	int result = Mix_PlayMusic(music, -1); //inf loops
	if(result == -1)
		ERROR("Mix_PlayMusic failed: '" << PATH << "': " << Mix_GetError());
	currentMusic = PATH;
}

void pauseMusic() {
	Mix_PauseMusic();
}

void resumeMusic() {
	Mix_ResumeMusic();
}

void stopMusic() {
	Mix_FadeOutMusic(1000); //One second fade out
	currentMusic = nullptr;
}

void pauseSoundEffects() {
	Mix_Pause(-1); //All
}

void resumeSoundEffects() {
	Mix_Resume(-1); //All
}

void stopSoundEffects() {
	Mix_HaltChannel(-1);
}

void stopAudioSystem() {
	for(auto wave:waves) {
		Mix_FreeChunk(wave.second);
	}
	Mix_CloseAudio();
	SDL_Quit();
}
