mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
cd ..
./build/Battleships #&> ./build/log.txt
