FROM ubuntu
RUN apt-get update && apt-get install -y locales && localedef -i en_US -c -f UTF-8 -A /usr/share/locale/locale.alias en_US.UTF-8
ENV LANG en_US.utf8

RUN apt-get -y install make
RUN apt-get -y install apt-utils
RUN	apt-get -y install  gcc
RUN	apt-get -y install  tcpdump
RUN	apt-get -y install  inetutils-ping
RUN	apt-get -y install  clang
# RUN rm -rf /var/lib/apt/lists/*

RUN mkdir /traceroute
WORKDIR /traceroute
ADD . /traceroute

ENTRYPOINT [ "/bin/bash" ]
