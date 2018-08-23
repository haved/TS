#!/usr/bash
./SoundFromSerial
git pull
make
while true; do
	./SoundFromSerial
	sleep 10
done
