#!bin/bash

set -u
set -e

PWD=`pwd`

FOLDER=cpp-viewshed-library
PACKAGE=viewshed
VERSION=$(grep "project("  CMakeLists.txt | grep -E -o -e "[0-9\.]+" | head -n 1)
DEBIAN_VERSION=$(grep "viewshed (" debian/changelog | grep -E -o -e "[0-9\.-]+" | head -n 1)

rm -rf build
cd ..
tar -acf "${PACKAGE}"_"${VERSION}".orig.tar.gz $FOLDER
cd $FOLDER
debuild -S -sa -d
cd ..
dput ppa:jancaha/gis-tools "${PACKAGE}"_"${DEBIAN_VERSION}"_source.changes
# rm $PACKAGE*