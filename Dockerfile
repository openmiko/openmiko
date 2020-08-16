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

# Copy the entire source tree. Expect it to be mounted over when developing.
COPY . /src


#COPY fs /openmiko/fs
#COPY legacy_src /openmiko/legacy_src
#COPY config /openmiko/config
#COPY patches /openmiko/patches
#COPY buildscripts /openmiko/buildscripts


RUN mkdir -p /openmiko/_build
WORKDIR "/openmiko/_build"
RUN wget https://buildroot.org/downloads/buildroot-2016.02.tar.gz;
RUN tar xvf buildroot-2016.02.tar.gz;
RUN rm buildroot-2016.02.tar.gz;

# Change to the buildroot directory
WORKDIR "/openmiko/_build/buildroot-2016.02"


RUN /src/buildscripts/setup_buildroot.sh

RUN make

WORKDIR /openmiko

ENTRYPOINT ["tail", "-f", "/dev/null"]
