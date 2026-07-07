# Mirrors OMSimulator/.CI/alpine/Dockerfile (the image used by the Jenkins
# `alpine` stage), plus a non-root user so the mounted workspace files keep the
# host user's ownership.
FROM alpine:3.24

RUN apk add -U \
  automake \
  cmake \
  g++ \
  git \
  libtool \
  libxcursor-dev \
  libxi-dev \
  libxinerama-dev \
  libxrandr-dev \
  linux-headers \
  make \
  mesa-dev \
  musl-dev \
  readline-dev \
  util-linux

ENV SHELL=/bin/bash

ARG USERNAME=openmodelica-user
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# Create the user. `shadow` provides useradd/groupadd, `bash` is a nicer login
# shell for VS Code, `sudo` lets you install more packages after connecting.
RUN apk add --no-cache bash shadow sudo \
    && groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m -s /bin/bash $USERNAME \
    && echo "$USERNAME ALL=(root) NOPASSWD:ALL" > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

ENV USER=$USERNAME
