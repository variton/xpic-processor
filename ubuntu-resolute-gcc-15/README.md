# Build the docker image
docker build -t ixpic:1.0 .

# Instance the container
docker run --name=ixpicc --hostname=cypher -v $PWD:/home/cxx-core --net=host --restart=no -it ixpic:1.0 /bin/bash
