sudo apt purge -y viewshed
sudo rm /usr/local/lib/libviewshed*
sudo rm -rf /usr/local/include/viewshed
sudo rm -rf /usr/local/lib/cmake/viewshed

cmake -S. -Bbuild -GNinja -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target all

cd build/tests 
ctest -VV
cd ../..

cd build
cpack -G DEB -C Release
# cd ../_packages
# sudo dpkg -i viewshed_*_amd64.deb
cd ..

sudo cmake --install build --config Release
