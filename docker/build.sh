cd ..

cmake -S. -Bbuild -G Ninja -DCMAKE_BUILD_TYPE=Release -DPACK_DEB:bool=on -DBUILD_DOCUMENTATION:bool=on -DBUILD_TESTS:bool=on
cmake --build build --config Release --target create_binaries
cmake --build build --config Release --target pack_deb

cd docker

mkdir install
cp ../_packages/viewshed_*.deb install/
cp ../build/_deps/simplerasters-build/src/*.so* install/ 

IMAGE_NAME=viewshed
DOCKER_IMAGE_NAME=cahik/viewshed:latest

docker build -t $IMAGE_NAME .

docker tag $IMAGE_NAME $DOCKER_IMAGE_NAME

docker login --username cahik

docker push $DOCKER_IMAGE_NAME

rm -rf install