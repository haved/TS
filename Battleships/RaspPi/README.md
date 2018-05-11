## Installing on Raspbian

- enable Serial in Rasberry Pi Configuration
- sudo apt-get update && sudo apt-get upgrade
- sudo apt-get install build-essential cmake

- mkdir Development

#### Getting audio to work (before building SDL_mixer)
- sudo chmod <user> -a -G audio
- sudo apt-get install libsmpeg-dev libogg-dev libvorbis-dev libmodplug-dev libmpg123-dev
 - Might not need smpeg anymore

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
Put the following in /etc/init.d/BattleshipsPullRun.sh
```
#!/bin/bash
### BEGIN INIT INFO
# Provides:             BattleshipsPullRun.sh
# Required-Start:       $all
# Required-Stop:
# Default-Start:        2 3 4 5
# Default-Stop:         0 1 6
# Short-Description:    Launch the pullAndRunForever script
### END INIT INFO

start() {
sleep 4
cd /home/pi/Development/TS/Battleships/RaspPi
nohup ./pullAndRunForever.sh > /dev/null 2>&1 &
echo "BattleshipsPullRun.sh has now started the process"
}

stop() {
pkill -f "./pullAndRunForever.sh"
killall Battleships
echo "BattleshipsPullRun.sh has now stopped the process"
}

case "$1" in
  start)
    start
	;;
  stop)
    stop
	;;
  restart)
    stop
	start
	;;
  *)
  echo "Usage: $0 <start|stop|restart>"
esac
```

`chmod +x` the file, and run the following command to make the service start at boot:
```
update-rc.d BattleshipsPullRun.sh defaults
```
