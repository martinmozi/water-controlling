FROM ubuntu:18.04              
WORKDIR /

COPY ./prerequisities.sh /tmp/prerequisities.sh

RUN apt-get update && \
    apt-get -y install git\
    git-core \
    wget \
    sudo \
    tar \
    gcc \
    gdb \
    g++ \
    cmake
	
RUN	chmod +x /tmp/prerequisities.sh && /tmp/prerequisities.sh
