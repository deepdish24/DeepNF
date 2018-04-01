FROM ubuntu:latest

MAINTAINER Akash Subramanian <skythelimit.96@gmail.com>

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get install -y g++ libpcap-dev && \
    apt-get install nlohmann-json-dev

COPY *

RUN bash build.sh
