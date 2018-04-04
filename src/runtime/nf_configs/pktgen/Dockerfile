FROM ubuntu:latest

MAINTAINER Akash Subramanian <skythelimit.96@gmail.com>

COPY pkt_sender.cpp .
COPY pkt_receiver.cpp .
COPY packet.h .
COPY packet.cpp .

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get install -y g++ libpcap-dev


RUN  g++ -std=c++11 -c packet.h packet.cpp && \
	 g++ -std=c++11 packet.cpp pkt_sender.cpp -o sender -I /usr/include/pcap -lpcap && \
	 g++ -std=c++11 packet.cpp pkt_receiver.cpp -o receiver -I /usr/include/pcap -lpcap
