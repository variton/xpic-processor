FROM debian:trixie

ENV INAME="cxx-core"

USER root
WORKDIR /root

RUN apt update
RUN apt upgrade -y
RUN apt install -y openssl
RUN apt install -y curl 
RUN apt install -y build-essential
RUN apt install -y vim 
RUN apt install -y doxygen
RUN apt install -y graphviz
RUN apt install -y gcc-14
RUN apt install -y valgrind 
RUN apt install -y heaptrack
RUN apt install -y ninja-build
RUN apt install -y cmake
RUN apt install -y clang
RUN apt install -y clang-tidy
RUN apt install -y clang-tools
RUN apt install -y clang-format
RUN apt install -y lcov
RUN apt install -y git
RUN apt install -y doctest
RUN apt install -y doctest-dev
RUN apt install -y libspdlog-dev
RUN apt install -y libjpeg-dev
RUN apt install -y libfmt-dev
RUN apt install -y libexpected-dev
RUN apt install -y libcli11-dev


ADD bashrc /root/.bashrc
ADD bash_aliases /root/.bash_aliases
ADD bash_env /root/.bash_env

WORKDIR /home/${INAME}
