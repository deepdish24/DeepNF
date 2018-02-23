FROM ubuntu:latest

MAINTAINER Akash Subramanian <skythelimit.96@gmail.com>

RUN apt-get update -y && \
    apt-get update -y && \
    apt-get install -y \
    openssh-server ethtool build-essential libpcap-dev libpcre3-dev libdumbnet-dev bison flex zlib1g-dev liblzma-dev openssl libssl-dev

# install DAQ
ENV DAQ_VERSION 2.0.6
RUN wget https://www.snort.org/downloads/snort/daq-${DAQ_VERSION}.tar.gz && \
    tar -zxvf daq-${DAQ_VERSION}.tar.gz && \
    cd daq-${DAQ_VERSION} && \
    ./configure; make; make install

# install Snort
ENV SNORT_VERSION 2.9.11.1
RUN wget https://www.snort.org/downloads/snort/snort-${SNORT_VERSION}.tar.gz \
    && tar xvfz snort-${SNORT_VERSION}.tar.gz \
    && cd snort-${SNORT_VERSION} \
    && ./configure --enable-sourcefire; make; make install

RUN ldconfig

# create a symlink
RUN ln -s /usr/local/bin/snort /usr/sbin/snort

# snort rules setup
RUN mkdir /etc/snort && \
    mkdir /etc/snort/preproc_rules && \
    mkdir /etc/snort/rules mkdir /var/log/snort  && \
    mkdir /usr/local/lib/snort_dynamicrules && \
    touch /etc/snort/rules/white_list.rules && \
    touch /etc/snort/rules/black_list.rules

RUN chmod -R 5775 /etc/snort/ && \
    chmod -R 5775 /var/log/snort/ && \
    chmod -R 5775 /usr/local/lib/snort && \
    chmod -R 5775 /usr/local/lib/snort_dynamicrules/

RUN cd snort-${SNORT_VERSION} && \
    cp -avr etc/*.conf etc/*.map etc/*.dtd /etc/snort/ && \
    cp -avr src/dynamic-preprocessors/build/usr/local/lib/snort_dynamicpreprocessor/* /usr/local/lib/snort_dynamicpreprocessor/

COPY local.rules /etc/snort/rules/
COPY snort.conf /etc/snort/

CMD ["echo", "Container successfully running!"]
#CMD ["snort", "-N", "-A", "console", "-q", "-c", "/etc/snort/snort.conf", "-Q", "-i", "eth1:eth2;"]
