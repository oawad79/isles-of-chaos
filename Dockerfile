FROM ubuntu:latest

ENV TZ=Europe/Minsk
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update && \
    apt-get install -y \
        git \
        openssh-server \
        libmysqlclient-dev \
        build-essential \
        cmake

RUN apt-get install -y libasound2-dev mesa-common-dev libx11-dev libxrandr-dev libxi-dev xorg-dev libgl1-mesa-dev libglu1-mesa-dev

# # Add the keys and set permissions
# RUN echo "$ssh_prv_key" > /root/.ssh/id_rsa && \
#     echo "$ssh_pub_key" > /root/.ssh/id_rsa.pub && \
#     chmod 600 /root/.ssh/id_rsa && \
#     chmod 600 /root/.ssh/id_rsa.pub

RUN git clone git@github.com:SkyVault/isles-of-chaos.git /isles-of-chaos/
WORKDIR /isles-of-chos/
