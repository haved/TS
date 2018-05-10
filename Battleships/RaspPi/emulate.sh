mkdir -p emulatingBuild
cd emulatingBuild
cmake .. -DCMAKE_BUILD_TYPE=Debug -DUSE_STDIO=YES &&
cmake --build . &&
cd ..
./../ArcadeSimulator/SimMain.py ./emulatingBuild/Battleships
