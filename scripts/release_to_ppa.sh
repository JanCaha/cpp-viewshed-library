#!bin/bash

set -u
set -e

PWD=`pwd`

FOLDER=cpp-viewshed-library
PACKAGE=viewshed
MAIN_VERSION=$(grep "project("  CMakeLists.txt | grep -E -o -e "[0-9\.]+" | head -n 1)
DEBIAN_VERSION=$(grep "viewshed (" debian/changelog -m 1 |  grep -E -o -e "[0-9]+\.[0-9]+\.[0-9]+(-[0-9]+)?")

rm -rf build
cd ..
tar -acf "$PACKAGE"_"$MAIN_VERSION".orig.tar.gz $FOLDER
cd $FOLDER
debuild -S -sa
cd ..
dput ppa:jancaha/gis-tools "$PACKAGE"_"$DEBIAN_VERSION"ppa0_source.changes
rm $PACKAGE*