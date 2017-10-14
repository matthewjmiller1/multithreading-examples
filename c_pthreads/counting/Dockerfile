FROM ubuntu:16.04

RUN apt-get update
RUN apt-get install -y make clang

ADD counting.c /
ADD Makefile /
RUN mkdir /obj

RUN make

CMD [ "/counting" ]
