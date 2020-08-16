FROM ubuntu:16.04

RUN \
  apt update && apt upgrade -y && \
  apt install -y \
  build-essential \
  git \
  autoconf \
  cmake \
  python \
  unzip \
  cpio \
  wget \
  vim \
  locales \
  lzop \
  bc \
  nano \
  libncurses5-dev \
  nfs-kernel-server \
  tftpd-hpa


RUN locale-gen --no-purge en_US.UTF-8

RUN mkdir -p /openmiko/build
WORKDIR "/openmiko/build"
RUN wget https://buildroot.org/downloads/buildroot-2016.02.tar.gz;
RUN tar xvf buildroot-2016.02.tar.gz;
RUN rm buildroot-2016.02.tar.gz;

# Change to the buildroot directory
WORKDIR "/openmiko/build/buildroot-2016.02"


# Copy the entire source tree. Expect it to be mounted over when developing.
COPY . /src


RUN /src/buildscripts/setup_buildroot.sh


RUN curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py
RUN python get-pip.py
RUN curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.35.3/install.sh | bash
RUN nvm install node

RUN npm config set user 0
RUN npm config set unsafe-perm true


# RUN make

# VOLUME /openmiko/build/buildroot-2016.02/output

WORKDIR /openmiko

ENTRYPOINT ["tail", "-f", "/dev/null"]
