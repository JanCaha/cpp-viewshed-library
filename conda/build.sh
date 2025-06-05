mkdir -p build
cmake -B build -S $SRC_DIR -G "Ninja" -DCMAKE_INSTALL_PREFIX=$PREFIX -DCMAKE_BUILD_TYPE=Release -DPACK_DEB:bool=off -DBUILD_DOCUMENTATION:bool=off -DBUILD_TESTS:bool=off -DCELL_EVENT_DATA_FLOAT:bool=on -DOUTPUT_RASTER_DATA_FLOAT:bool=on -DNEEDS_QT:bool=on -DBUILD_GUI_APP:bool=on
cmake --build build --config Release
cmake --install build --prefix $PREFIX
