#!/bin/bash

set -e

sudo rm -f /usr/local/bin/inverseviewshed /usr/local/bin/viewshed /usr/local/bin/viewshedcalculator /usr/local/bin/losextractor
sudo rm -f /usr/local/lib/libviewshed.so /usr/local/lib/libviewshed.so.* /usr/local/lib/libviewshedwidgets.so /usr/local/lib/libviewshedwidgets.so.*
sudo rm -rf /usr/local/include/Viewshed
sudo rm -rf /usr/local/lib/cmake/Viewshed
