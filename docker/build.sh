mkdir install
cp ../_packages/viewshed_*.deb install/

docker build -t viewshed .
rm -rf install