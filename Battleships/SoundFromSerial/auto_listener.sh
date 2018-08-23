#!/bin/bash
#Append following line to .config/lxsession/LXDE-pi/autostart
#@lxterminal -l -e "(cd /home/pi/Development/TS/Battleships/SoundFromSerial ; bash ./auto_listener.sh ; read)"
./SoundFromSerial
git pull
make
while true; do
	./SoundFromSerial
	sleep 10
done
