FROM phusion/baseimage
MAINTAINER Jack Bradach <jack@bradach.net>

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y \
        dfu-util \
        g++ \
        gcc \
        git \
        make \
        ncurses-dev \
        python2.7 \
        python3-ply \
        python3-yaml \
        xz-utils

# Change these to adjust versions.  They're below the package installation
# line to avoid building extra layers.
ENV ZEPHYR_KERNEL_VERSION 1.7.0
ENV ZEPHYR_SDK_VERSION 0.9

# Set up Zephyr toolchain
WORKDIR /tmp
RUN curl -fSL \
    "https://github.com/zephyrproject-rtos/meta-zephyr-sdk/releases/download/$ZEPHYR_SDK_VERSION/zephyr-sdk-$ZEPHYR_SDK_VERSION-setup.run" \
    -o zephyr-sdk.run \
    && sh ./zephyr-sdk.run \
    && rm ./zephyr-sdk.run
ENV ZEPHYR_GCC_VARIANT zephyr
ENV ZEPHYR_SDK_INSTALL_DIR /opt/zephyr-sdk

# Check out Zephyr
WORKDIR /opt
RUN git clone --depth 1 https://github.com/zephyrproject-rtos/zephyr.git -b v$ZEPHYR_KERNEL_VERSION \
    && /bin/bash -c "source /opt/zephyr/zephyr-env.sh"

RUN mkdir /src
WORKDIR /src

CMD ["make x86 && make arc"]