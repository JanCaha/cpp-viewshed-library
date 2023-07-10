set -u
set -e

PWD=`pwd`

cd docker

IMAGE_NAME=viewshed
DOCKER_IMAGE_NAME=cahik/viewshed:latest

docker build -t $IMAGE_NAME .

docker tag $IMAGE_NAME $DOCKER_IMAGE_NAME

docker login --username cahik

docker push $DOCKER_IMAGE_NAME
