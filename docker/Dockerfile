FROM fedora:latest

RUN dnf -y --setopt=tsflags=nodocs update && \
    dnf -y --setopt=tsflags=nodocs install \
        arm-none-eabi-binutils-cs \
        arm-none-eabi-gcc-cs \
        arm-none-eabi-gcc-cs-c++ \
        arm-none-eabi-newlib \
        avr-binutils \
        avr-gcc \
        avr-gcc-c++ \
        avr-libc \
        avrdude \
        boost-devel \
        dfu-programmer \
        dfu-util \
        gcc \
        gcc-c++ \
        git \
        make \
        patch \
        wget \
        zip unzip \
    && dnf -y clean all

VOLUME  /build
WORKDIR /build
CMD make
