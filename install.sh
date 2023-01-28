# sudo apt purge -y viewshed

sudo cmake --build build --target uninstall

cmake -S. -Bbuild -G Ninja -DCMAKE_BUILD_TYPE=Release -DPACK_DEB=off
cmake --build build --config Release --target viewshed

# testing
cmake --build build --config Release --target build_tests
cmake --build build --config Release --target run_tests

# install
cmake --build build --config Release --target install