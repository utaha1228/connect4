FROM ubuntu:22.04

WORKDIR /connect4
COPY . .

RUN ls

RUN apt update
RUN apt upgrade -y

RUN apt-get install -y python3
RUN apt-get install -y python3-pip
RUN apt install g++

RUN pip3 install Flask

RUN g++ src/test.cpp -Ofast -o ./test

CMD ["python3","./server.py"]

EXPOSE 8080
