# sudo apt purge -y viewshed
# sudo cmake --build build --target uninstall
cmake --build build --target clean

cmake -S. \
    -Bbuild \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DPACK_DEB:bool=off \
    -DBUILD_DOCUMENTATION:bool=off \
    -DBUILD_TESTS:bool=on \
    -DCELL_EVENT_DATA_FLOAT:bool=on \
    -DOUTPUT_RASTER_DATA_FLOAT:bool=on

cmake --build build --config Release --target viewshed

# testing
# cmake --build build --config Debug --target run_tests
cmake --build build --config Release --target all
# cmake --build build --config Release --target run_tests

# install
sudo cmake --build build --config Release --target install

# pack DEB
# cmake -S. -Bbuild -G Ninja -DCMAKE_BUILD_TYPE=Release -DPACK_DEB:bool=on -DBUILD_DOCUMENTATION:bool=off -DBUILD_TESTS:bool=off
# cmake --build build --config Release --target all
# cmake --build build --config Release --target pack_viewshed_library_deb

# sudo cmake --build build --target clean
# cmake -S. -Bbuild -G Ninja -DCMAKE_BUILD_TYPE=Release \
#     -DPACK_DEB:bool=off \
#     -DBUILD_DOCUMENTATION:bool=off \
#     -DBUILD_TESTS:bool=on \
#     -DCELL_EVENT_DATA_FLOAT:bool=off \
#     -DOUTPUT_RASTER_DATA_FLOAT:bool=off
# cmake --build build --config Release --target run_tests
