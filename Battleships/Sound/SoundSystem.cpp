#include "SoundSystem.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <iostream>

#define ERROR(s) do{std::cerr << "AudioSystem:" << __LINE__ << ": " << s << std::endl; return;}while(false)

#include "soundpaths.hpp"

Mix_Chunk *waves[SOUND_COUNT];
Mix_Music *music[MUSIC_COUNT];

char TMP_PATH_CONCAT[255];
void startAudioSystem(const char* pathPrefix) {
	if(SDL_Init(SDL_INIT_AUDIO) != 0)
		ERROR("Failed to init SDL for audio: " << SDL_GetError());
	if(Mix_OpenAudio(44100, AUDIO_S32LSB, 2, 1024) == -1)
		ERROR("Failed to Mix_OpenAudio" << Mix_GetError());

	for(int wave = 0; wave < SOUND_COUNT; wave++) {
		sprintf(TMP_PATH_CONCAT, "%seffects/%s", pathPrefix, SOUND_PATHS[wave]);
		waves[wave] = Mix_LoadWAV(TMP_PATH_CONCAT);
		if(!waves[wave])
			ERROR("Failed loading wave: '" << TMP_PATH_CONCAT << "': " << Mix_GetError());
	}


	for(int muse = 0; muse < MUSIC_COUNT; muse++) {
		sprintf(TMP_PATH_CONCAT, "%smusic/%s", pathPrefix, MUSIC_PATHS[muse]);
		music[muse] = Mix_LoadMUS(MUSIC_PATHS[muse]);
		if(!music[muse])
			ERROR("Failed loading music: '" << TMP_PATH_CONCAT << "': " << Mix_GetError());
	}
}

void stopAudioSystem() {
	Mix_CloseAudio();
	SDL_Quit();
}

void playSoundEffect(SoundCode soundCode) {
	int result = Mix_PlayChannel(-1, waves[soundCode], 0); //0 loops
	if(result == -1) {
		stopSoundEffects(); //All channels full
		result = Mix_PlayChannel(-1, waves[soundCode], 0);
	}
	if(result == -1)
		ERROR("Mix_PlayChannel failed: '" << SOUND_PATHS[soundCode] << "': " << Mix_GetError());
}

void pauseSoundsEffects() {
	Mix_Pause(-1); //All
}

void resumeSoundEffects() {
	Mix_Resume(-1); //All
}

void stopSoundEffects() {
	Mix_HaltChannel(-1); //All
}

MusicCode currentMusicCode = MUSIC_COUNT;
void loopMusic(MusicCode musicCode) {
	if(musicCode == currentMusicCode) {
		resumeMusic();
		return;
	}
	currentMusicCode = musicCode;
	Mix_PlayMusic(music[musicCode], -1); //inf loop
}

void pauseMusic() {
	Mix_PauseMusic();
}

void resumeMusic() {
	Mix_ResumeMusic();
}

void fadeOutMusic(int millis) {
	Mix_FadeOutMusic(millis);
	currentMusicCode = MUSIC_COUNT;
}