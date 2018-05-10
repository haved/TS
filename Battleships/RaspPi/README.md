## Installing on Raspbian

- enable Serial in Rasberry Pi Configuration
- sudo apt-get update && sudo apt-get upgrade
- sudo apt-get install build-essential cmake

- mkdir Development

#### SDL2
- cd ~/Development
- wget https://www.libsdl.org/release/SDL2-2.0.8.tar.gz
- tar -xzf SDL2-2.0.8.tar.gz
- cd SDL2-2.0.8
- mkdir build && cd build
- ../configure --host=armv71-raspberry-linux-gnueabihf --disable-esd --disable-video-mir --disable-video-wayland --disable-video-x11 --disable-video-opengl
- make -j4
- sudo make install

#### SDL2_mixer
- cd ~/Development
- wget https://www.libsdl.org/projects/release/SDL2_mixer-2.0.2.tar.gz
- tar -xzf SDL2_mixer-2.0.2.tar.gz 
- cd SDL2_mixer-2.0.2
- mkdir build && cd build
- ../configure
- make -j4
- sudo make install

#### Battleships
- cd ~/Development
- git clone http://github.com/haved/TS
- cd TS/Battleships/RaspPi
- ./compileAndRun.sh

#### Getting serial to work
I don't think you need, say, arduino IDE installed for Serial to work, but it was a nice debugging tool, seing as our program only recieved serial input upon starting the program, at which point all input came at once.
I think I fixed this by stealing a bunch of termios flags from pyserial.

#### Autostart without a desktop environment
