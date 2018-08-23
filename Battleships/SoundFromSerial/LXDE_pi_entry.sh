#!/bin/bash
#Append following line to .config/lxsession/LXDE-pi/autostart (without the # symbol)
#lxterminal -l -e /home/pi/Development/TS/Battleships/SoundFromSerial/LXDE_pi_entry.sh
sleep 4
cd /home/pi/Development/TS/Battleships/SoundFromSerial/
bash ./auto_listener.sh
