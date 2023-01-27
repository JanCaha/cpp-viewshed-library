# sudo apt purge -y viewshed

sudo cmake --build build --target uninstall

cmake -S. -Bbuild -GNinja -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target viewshed

# testing
cmake --build build --config Release --target build_tests
cmake --build build --config Release --target run_tests

cd build
cpack -G DEB -C Release
# cd ../_packages
# sudo dpkg -i viewshed_*_amd64.deb
cd ..

# install
cmake --build build --config Release --target install