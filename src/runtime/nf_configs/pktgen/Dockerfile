FROM ubuntu:latest

MAINTAINER Akash Subramanian <skythelimit.96@gmail.com>

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get install software-properties-common -y && \
	add-apt-repository ppa:openjdk-r/ppa && apt-get update -y && \
    apt-get install -y g++ telnet openjdk-8-jdk

COPY Receiver.java .
COPY Sender.java .
COPY build_java.sh .


RUN  bash build_java.sh
