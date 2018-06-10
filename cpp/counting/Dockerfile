FROM ubuntu:latest

RUN apt-get update
RUN apt-get install -y make clang cmake

RUN mkdir /project
RUN mkdir /project/src
RUN mkdir /project/build

ADD CMakeLists.txt /project
ADD src/counting.cpp /project/src

WORKDIR /project/build
RUN cmake ..
RUN make

CMD [ "/project/bin/counting" ]
