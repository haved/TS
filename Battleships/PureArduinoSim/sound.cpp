#include "sound.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <iostream>

#define ERROR(s) do{std::cerr << "AudioSystem:" << __LINE__ << ": " << s << std::endl; return;}while(false)

#include "../Sound/soundcodes.hpp"
#include "../Sound/soundpaths.hpp"

Mix_Chunk *waves[SOUND_COUNT];
Mix_Music *music[MUSIC_COUNT];

void startAudioSystem() {
	if(SDL_Init(SDL_INIT_AUDIO) != 0)
		ERROR("Failed to init SDL for audio: " << SDL_GetError());
	if(Mix_OpenAudio(44100, AUDIO_S32LSB, 2, 1024) == -1)
		ERROR("Failed to Mix_OpenAudio" << SDL_GetError());


}

void stopAudioSystem() {
	
}

void playSoundEffect(int soundCode) {

}

void pauseSoundsEffects() {

}

void resumeSoundEffects() {

}

void stopSoundEffects() {

}

MusicCode currentMusicCode = MUSIC_COUNT;
void loopMusic(MusicCode musicCode) {
	if(musicCode == currentMusicCode)
		return;
	currentMusicCode = musicCode;
}

void pauseMusic() {

}

void resumeMusic() {

}

void fadeOutMusic(int millis) {
	currentMusicCode = MUSIC_COUNT;
}
