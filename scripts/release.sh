FOLDER=cpp-viewshed-library
PACKAGE=viewshed
VERSION=$(grep "project("  CMakeLists.txt | grep -E -o -e "[0-9\.]+" | head -n 1)
DEBIAN_VERSION=$(grep "viewshed (" debian/changelog | grep -E -o -e "[0-9\.-]+" | head -n 1)

cmake \
    -S. \
    -B build \
    -G Ninja \
    -DCMAKE_CXX_COMPILER=clang++-13 \
    -D CMAKE_INSTALL_PREFIX=debian/tmp/ \
    -D CMAKE_BUILD_TYPE=Release \
    -D BUILD_TESTS:bool=off \
    -D CMAKE_INSTALL_PREFIX=/usr

cmake --build build --config Release --target all
# cmake --build build --config Release --target install

cd ..
dir
tar -acf "${PACKAGE}"_"${DEBIAN_VERSION}".orig.tar.gz $FOLDER
ls -l

cd $FOLDER

debuild -S -sa -d

cd ..

dput ppa:jancaha/gis-tools "${PACKAGE}"_"${DEBIAN_VERSION}"_source.changes