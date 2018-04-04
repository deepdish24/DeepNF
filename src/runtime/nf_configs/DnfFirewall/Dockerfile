FROM ubuntu:latest

MAINTAINER Akash Subramanian <skythelimit.96@gmail.com>

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get install -y g++ libpcap-dev

COPY dnf_firewall.cpp .
COPY packet.h .
COPY packet.cpp .

RUN g++ -std=c++11 -c packet.h packet.cpp && \
	g++ -std=c++11 dnf_firewall.cpp -o fw -I /usr/include/pcap -lpcap
