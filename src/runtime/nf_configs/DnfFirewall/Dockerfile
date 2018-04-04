FROM ubuntu:latest

MAINTAINER Akash Subramanian <skythelimit.96@gmail.com>

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get install -y g++ libpcap-dev

COPY packet.h .
COPY packet.cpp .
COPY dnf_firewall.cpp .

RUN  g++ -std=c++11 -c packet.h packet.cpp
RUN g++ -std=c++11 packet.h packet.cpp dnf_firewall.cpp \
	-o fw -I /usr/include/pcap -lpcap
