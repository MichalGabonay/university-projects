MAINTAINER Martin Kocour
FROM ubuntu:16.04

RUN add-apt-repository ppa:sumo/stable && apt-get update
RUN apt-get install -y \
        build-essential \
        tcl \
        wget \
        curl \
        vim \
        python \
        pytho3 \
        python-tk \
        python3-tk \
        python-pip \
        python3-pip \
        sumo \
        sumo-tools \
        sumo-doc

#Install REDIS
WORKDIR /opt
RUN wget http://download.redis.io/redis-stable.tar.gz && \
        tar xvzf redis-stable.tar.gz && \
        cd redis-stable && make && make test && make install

RUN
