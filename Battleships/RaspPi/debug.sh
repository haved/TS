mkdir -p debugBuild
cd debugBuild
cmake .. -DCMAKE_BUILD_TYPE=Debug -DUSE_STDIO=YES &&
cmake --build . &&
cd ..
./../ArcadeSimulator/SimMain.py ./debugBuild/Battleships
