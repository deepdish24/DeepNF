FROM ubuntu:latest

MAINTAINER Akash Subramanian <skythelimit.96@gmail.com>

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get install -y g++ libpcap-dev && \
    apt install linuxbrew-wrapper -y && \
    chmod 777 /home/ && \
    chmod 777 /

COPY * ./


USER appuser
