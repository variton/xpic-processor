# Build the docker image
docker build -t izattoo:1.0 .

# Instance the container
docker run --name=zattooc --hostname=cypher -v $PWD:/home/cxx-core --net=host --restart=no -it izattoo:1.0 /bin/bash
