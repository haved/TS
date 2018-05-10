mkdir -p debugBuild
cd debugBuild
cmake .. -DCMAKE_BUILD_TYPE=Debug -DDEBUG_IO=YES &&
cmake --build . &&
./Battleships
