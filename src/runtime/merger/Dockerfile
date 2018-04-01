FROM ubuntu:latest

MAINTAINER Akash Subramanian <skythelimit.96@gmail.com>

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get install -y g++ libpcap-dev && \
    apt-get install cmake && \
    apt-get install python3 -y --fix-missing && \
    apt-get install python-pip python-dev build-essential && \
    pip install -U pip && pip install cget && \
    cget install nlohmann/json@v3.1.0



COPY *

RUN bash build.sh
