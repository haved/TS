#!/bin/bash
#This file needs to be run in the current directory, launch LXDE_pi_entry.sh from autostart
./SoundFromSerial
make clean
git pull
make
while true; do
	./SoundFromSerial
	sleep 10
done
