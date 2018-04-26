mkdir -p debugBuild
cd debugBuild
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
./Battleships
