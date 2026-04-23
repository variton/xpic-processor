# Pic Processor

## Quick Start
1- Launch the docker container

2- Execute the local C.I:

```
./local-ci.sh

```
3- Performance tests

clean the build:
```
./build.sh clean

```

in release:
```
./build.sh release

```

check the time to deinterlace the given image:
```
time ./deinterlacer -i rc/interlaced.jpg -o output/deinterlaced.jpg

```

## Documentation
- \ref docker
- \ref build
- \ref tests 
