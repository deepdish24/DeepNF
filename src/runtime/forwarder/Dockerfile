FROM ubuntu:latest

MAINTAINER Victoria Xiao

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get install -y g++ libpcap-dev && \
    apt-get install build-essential -y && apt install wget -y && \
    apt-get install git && \
    wget https://cmake.org/files/v3.11/cmake-3.11.0.tar.gz && \
    tar xvf cmake-3.11.0.tar.gz && \
    cd cmake-3.11.0 && ./bootstrap && make && make install

ADD . DeepNF

RUN cd DeepNF && rm -rf build && mkdir build && cd build && \
    cmake ../ && make google_protobuf && make