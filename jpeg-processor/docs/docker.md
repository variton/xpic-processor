# Docker {#docker}

Prepare the docker image to develop and test jpeg-processor
 
```
cd ubuntu-resolute-gcc-15

```
1. Generate the docker image

```
docker build -t ixpic:1.0 .

```
2. Launch the docker container
*remark:*
    *execute the following command from the jpeg-processor root directory*

```
docker run --name=xpicc --hostname=cypher -v $PWD:/home/cxx-core --net=host --restart=no -it ixpic:1.0 /bin/bash

```
