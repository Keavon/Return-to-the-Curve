rm -r build
mkdir build
cd build
cmake ..
make -j
./curve
cd ..