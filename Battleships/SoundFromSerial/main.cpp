#include "sound.hpp"
#include "SerialIO.hpp"
#include <iostream>

int main() {
	startAudioSystem("../Sound/");

	SerialIO serial;
	while(true) {
		char c = serial.waitForByte();
		if(c != '>')
			continue;
		char command = serial.waitForByte();
		switch(command) {
		case 's': //play (code)
			{
				char sound = serial.waitForByte();
				std::cout << "Playing sound effect: " << (int)sound << std::endl;
				if(sound < SOUND_COUNT)
					playSoundEffect(static_cast<SoundCode>(sound));
				else
					std::cerr << "Asked to play illegal sound code" << (int)sound << std::endl;
			}
			break;

		case 'p': //pause all
			pauseSoundEffects();
			break;

		case 'r': //Resume all
			resumeSoundEffects();
			break;

		case 'S': //stop all
			stopSoundEffects();
			break;

		case 'M': //loop music(code)
			{
				char music = serial.waitForByte();
				if(music < MUSIC_COUNT)
				    loopMusic(static_cast<MusicCode>(music));
				else
					std::cerr << "Asked to play illegal music code" << (int)music << std::endl;
			}
			break;

		case 'P': //pause music
		    pauseMusic();
			break;

		case 'R': //Resume music
			resumeSoundEffects();
			break;

		case 'F': //Fade out music (millis:byte)
			fadeOutMusic(serial.waitForByte());
			break;

		default:
			std::cerr << "Got Illegal sound instruction code: " << command << std::endl;
			break;
		}
	}

	stopAudioSystem();
	return 0;
}
