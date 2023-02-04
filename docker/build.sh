mkdir lib
cp ../build/libviewshed* lib/

mkdir bin
cp ../build/inverseviewshed bin/
cp ../build/viewshed bin/
cp ../build/viewshedcalculator bin/

docker build -t viewshed .