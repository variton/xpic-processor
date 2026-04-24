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
    *execute the following command from the pic-processor directory*

```
docker run --name=zattooc --hostname=cypher -v $PWD:/home/cxx-core --net=host --restart=no -it izattoo:1.0 /bin/bash

```

### Environment to develop the pic-processor 

#### Build & compile pic-processor (Quick start)
*remark:*
    *execute the following command from the /home/cxx-core directory in the docker container*

execute the local C.I:

```
./local-ci.sh

```
*remark:*
    *the command line above has generated the reports & documentation as well*

#### Get the help menu of the deinterlacer binary 
*remark:*
    *execute the following command from the /home/cxx-core directory in the docker container*

get the help menu of the deinterlacer binary:

```
deinterlacer --help

``` 
#### Execute the deinterlacer binary
*remark:*
    *execute the following command from the /home/cxx-core directory in the docker container*

get the help menu of the deinterlacer binary:

```
deinterlacer -i rc/interlaced.jpg -o output/deinterlaced.jpg 

``` 

#### Check the logs
*remark:*
    *execute the following command from the /home/cxx-core directory in the docker container*

check the logs of the deinterlacer binary:

```
cat logs/deinterlacer.log

``` 

#### Read the documentation:
*remark 1:*
    *execute the following command from the /home/cxx-core directory in the docker container*

    *to generate the reports you must execute first the execute the following command from the /home/cxx-core directory in the docker container*

generate all the 

check the reports and documentation:

```
cd reports; ls

``` 

read the reports and documentation:
*remark:*
    *open your browser and add the url of the different files*

open the coverage tests:

```
file:///<your path to the pic-processor folder>/reports/coverage_html/index.html

``` 

open the doxygen documentation:

```
file:///<your path to the pic-processor folder>/reports/html/index.html

``` 

open the valgrind reports:

```
file:///<your path to the pic-processor folder>/reports/valgrind-report.html

``` 

open the deinterlacer quality report[raw valgrind report]:

```
file:///<your path to the pic-processor folder>/reports/deinterlacer-quality-report.log

```


