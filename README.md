# Xpic-processor
Cli and tools to process images files.
A cpp wrapper around libjpeg to process images files.
It is intended to provide a simple cli to deinterlace images.

## Quick start
### Prepare environment
Prepare the docker image to develop and test jpeg-processor
 
```
cd ubuntu-resolute-gcc-15

```
#### Generate the docker image

```
docker build -t ixpic:1.0 .

```
#### Launch the docker container
*remark:*
    *execute the following command from the jpeg-processor directory*

```
docker run --name=ixpicc --hostname=cypher -v $PWD:/home/cxx-core --net=host --restart=no -it ixpic:1.0 /bin/bash

```

### Environment to develop the jpeg-processor 

#### Build & compile jpeg-processor (Quick start)
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


check the reports and documentation:

```
cd reports; ls

``` 


read the reports and documentation:
*remark 2:*
    *open your browser and add the url of the different files*

open the coverage tests:

```
file:///<your path to the jpeg-processor folder>/reports/coverage_html/index.html

``` 


open the doxygen documentation:

```
file:///<your path to the jpeg-processor folder>/reports/html/index.html

``` 


open the valgrind reports:

```
file:///<your path to the jpeg-processor folder>/reports/valgrind-report.html

``` 


open the deinterlacer quality report[raw valgrind report]:

```
file:///<your path to the jpeg-processor folder>/reports/deinterlacer-quality-report.log

```
