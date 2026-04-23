# PicProcessor
Cli and tools to process images files
A cpp wrapper around libjpeg to process images files.
it is intended to provide a simple cli to deinterlace images

## Quick start
### Prepare environment
Prepare the docker image to develop and test pic-processor
 
```
cd debian-trixie-gcc-14

```
#### Generate the docker image

```
docker build -t izattoo:1.0 .

```
#### Launch the docker container
*remark:*
    *execute the following command from the pic-processor root directory*

```
docker run --name=zattooc --hostname=cypher -v $PWD:/home/cxx-core --net=host --restart=no -it izattoo:1.0 /bin/bash

```

### Environment to develop the pic-processor 

#### Build & compile pic-processor 
*remark:*
    *execute the following command from the pic-processor root directory in the docker container*

in debug:

```
./build.sh debug

```
in release:

```
./build.sh release

```
