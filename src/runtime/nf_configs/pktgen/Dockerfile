FROM ubuntu:latest

MAINTAINER Akash Subramanian <skythelimit.96@gmail.com>

COPY pkt_sender.cpp .
COPY pkt_receiver.cpp .

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get install -y g++

RUN  g++ -std=c++11 pkt_sender.cpp -o sender && \
	 g++ -std=c++11 pkt_receiver.cpp -o receiver 
