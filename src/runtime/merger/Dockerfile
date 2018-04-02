FROM ubuntu:latest

MAINTAINER Akash Subramanian <skythelimit.96@gmail.com>

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get install -y g++ libpcap-dev && \
    apt install linuxbrew-wrapper -y && \
    apt-get install build-essential -y && \
    echo 'export PATH="/home/linuxbrew/.linuxbrew/bin:$PATH"' >>~/.profile && \
    echo 'export MANPATH="/home/linuxbrew/.linuxbrew/share/man:$MANPATH"' >>~/.profile && \
    echo 'export INFOPATH="/home/linuxbrew/.linuxbrew/share/info:$INFOPATH"' >>~/.profile && \
    PATH="/home/linuxbrew/.linuxbrew/bin:$PATH" && \
    brew install gcc && \
    chmod 777 /home/ && \
    chmod 777 /

COPY ActionTableHelper.cpp .
COPY ActionTableHelper.h .
COPY ConflictItem.cpp .
COPY ConflictItem.h .
COPY Field.h .
COPY Field.cpp .
COPY merge.cpp .
COPY Field.h .
COPY Field.cpp .
COPY MergerInfo.h .
COPY MergerInfo.cpp .
COPY NF.h .
COPY NF.cpp .
COPY packet.h .
COPY packet.cpp .
COPY RuntimeNode.h .
COPY RuntimeNode.cpp .

