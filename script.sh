#!/bin/bash

make create_docker
IMAGE_NAME="my-ftp-server"

# Stop and remove containers with the specific image name
for container_id in $(docker ps -a --filter "ancestor=$IMAGE_NAME" -q); do
    docker stop $container_id
    docker rm $container_id
done
make run_docker