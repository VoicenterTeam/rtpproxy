FROM debian:bullseye-slim
WORKDIR /home/rtpproxy
COPY . ./
RUN ls /home/rtpproxy
RUN apt update -y &&  apt install -y git gcc make pkg-config  libsndfile1-dev libsrtp2-dev libbcg729-dev  libsystemd-dev  tcpdump sudo git libssl-dev
COPY ./docker/docker-entrypoint.sh ./docker-entrypoint.sh

RUN  git -C . submodule update --init --recursive
RUN  ./configure &&   make clean all &&   make install
#ENTRYPOINT ["/bin/bash", "-c", "./starter.sh"]
ENTRYPOINT ["/bin/bash", "-c", "docker-entrypoint.sh"]

