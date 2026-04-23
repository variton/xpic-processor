# Docker {#docker}

Prepare the docker image to develop and test pic-processor
 
```
cd debian-trixie-gcc-14

```
1. Generate the docker image

```
docker build -t izattoo:1.0 .

```
2. Launch the docker container
*remark:*
    *execute the following command from the pic-processor root directory*

```
docker run --name=zattooc --hostname=cypher -v $PWD:/home/cxx-core --net=host --restart=no -it izattoo:1.0 /bin/bash

```
