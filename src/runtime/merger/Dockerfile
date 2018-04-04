FROM ubuntu:latest

MAINTAINER Akash Subramanian <skythelimit.96@gmail.com>

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get install -y g++ libpcap-dev


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
COPY MergerOperator.h .
COPY MergerOperator.cpp .
COPY NF.h .
COPY NF.cpp .
COPY packet.h .
COPY packet.cpp .
COPY RuntimeNode.h .
COPY RuntimeNode.cpp .

COPY build.sh .

RUN bash build.sh
