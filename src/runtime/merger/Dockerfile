FROM ubuntu:latest

MAINTAINER Akash Subramanian <skythelimit.96@gmail.com>

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get install -y g++ libpcap-dev

COPY merge.cpp .
COPY RuntimeNode.h .
COPY RuntimeNode.cpp .
COPY NF.h .

RUN  g++ -std=c++11 -c RuntimeNode.h
RUN  g++ -std=c++11 -c RuntimeNode.h RuntimeNode.cpp
RUN g++ -std=c++11 RuntimeNode.h RuntimeNode.cpp merge.cpp \
	-o merge -I /usr/include/pcap -lpcap
