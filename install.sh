sudo apt purge -y viewshed

sudo /usr/bin/cmake --build /home/cahik/Codes/C++/ViewshedLibrary/build --config Release # --target install --
cd build
cpack -G DEB
cd ../_packages
sudo dpkg -i viewshed_*_amd64.deb

# cd build
# cmake .. 
# make 
# sudo make install