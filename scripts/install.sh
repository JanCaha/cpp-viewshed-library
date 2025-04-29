cmake --build build --target clean

cmake \
    -S . \
    -B build \
    -G Ninja \
    -D CMAKE_CXX_COMPILER=clang++ \
    -D CMAKE_BUILD_TYPE=Release \
    -D PACK_DEB:bool=ON \
    -D BUILD_DOCUMENTATION:bool=off \
    -D BUILD_TESTS:bool=off \
    -D CELL_EVENT_DATA_FLOAT:bool=on \
    -D OUTPUT_RASTER_DATA_FLOAT:bool=on \
    -D CMAKE_INSTALL_PREFIX=/usr \
    -D NEEDS_QT:bool=on \
    -D BUILD_GUI_APP:bool=on

cmake --build build --config Release --target library_viewshed
cmake --build build --config Release --target create_binaries

# install
sudo cmake --build build --config Release --target install
