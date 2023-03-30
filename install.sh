# sudo apt purge -y viewshed
# sudo cmake --build build --target uninstall

cmake -S. -Bbuild -G Ninja -DCMAKE_BUILD_TYPE=Release -DPACK_DEB:bool=off -DBUILD_DOCUMENTATION:bool=off -DBUILD_TESTS:bool=on

cmake --build build --config Release --target viewshed

# testing
# cmake --build build --config Debug --target run_tests
cmake --build build --config Release --target run_tests

# install
sudo cmake --build build --config Release --target install

# pack DEB
# cmake -S. -Bbuild -G Ninja -DCMAKE_BUILD_TYPE=Release -DPACK_DEB:bool=on -DBUILD_DOCUMENTATION:bool=on -DBUILD_TESTS:bool=on
# cmake --build build --config Release --target pack_deb

