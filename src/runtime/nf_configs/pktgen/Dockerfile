FROM ubuntu:latest

MAINTAINER Akash Subramanian <skythelimit.96@gmail.com>

COPY pkt_sender.cpp .
COPY pkt_receiver.cpp .
COPY packet.h .
COPY packet.cpp .
COPY socket_util.h .
COPY socket_util.cpp .
COPY address_util.h .
COPY address_util.cpp .
COPY address.h .


RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get install -y g++


RUN g++ -std=c++11 -c address.h address_util.h address_util.cpp && \
	g++ -std=c++11 -c packet.h packet.cpp && \
	g++ -std=c++11 -c packet.cpp socket_util.h socket_util.cpp && \
	g++ -std=c++11 packet.cpp socket_util.cpp address_util.cpp pkt_sender.cpp -o sender && \
	g++ -std=c++11 packet.cpp socket_util.cpp address_util.cpp pkt_receiver.cpp -o receiver
