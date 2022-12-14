FROM ubuntu:22.04

WORKDIR /connect4
COPY . .

RUN ls

RUN apt update
RUN apt upgrade -y

RUN apt-get install -y python3
RUN apt-get install -y python3-pip

RUN pip3 install Flask

CMD ["python3","./server.py"]

EXPOSE 8080
