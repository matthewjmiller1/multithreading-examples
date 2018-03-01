FROM ubuntu:latest

RUN apt-get update
RUN apt-get install -y make clang cmake

RUN mkdir /src
RUN mkdir /build

ADD CMakeLists.txt /
ADD src/counting.cpp /src

WORKDIR /build
RUN cmake ..
RUN make

CMD [ "/bin/counting" ]
