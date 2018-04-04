FROM ubuntu:latest

MAINTAINER Akash Subramanian <skythelimit.96@gmail.com>

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get install -y g++

COPY dnf_firewall.cpp .

RUN g++ -std=c++11 dnf_firewall.cpp -o fw
