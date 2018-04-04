FROM ubuntu:latest

MAINTAINER Akash Subramanian <skythelimit.96@gmail.com>

COPY pkt_sender.cpp
COPY pkt_receiver.cpp

RUN  g++ pkt_sender.cpp -o sender && \
	 g++ pkt_receiver.cpp -o receiver 
