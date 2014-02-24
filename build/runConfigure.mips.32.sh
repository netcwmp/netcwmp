#!/bin/bash

export PATH=/usr/local/mip32-gcc-buildroot/bin:$PATH

GCC_DIR=/usr/local/mip32-gcc-buildroot
BUILD_DIR=/home/users/summer/source/opensrc/trunk/netcwmp-1.2.1


AR=mipsel-linux-uclibc-ar LD=mipsel-linux-uclibc-ld NM=mipsel-linux-uclibc-nm CC="mipsel-linux-uclibc-gcc" GCC="mipsel-linux-uclibc-gcc" CXX=mipsel-linux-uclibc-g++ RANLIB=mipsel-linux-uclibc-ranlib STRIP=mipsel-linux-uclibc-strip OBJCOPY=mipsel-linux-uclibc-objcopy OBJDUMP=mipsel-linux-uclibc-objdump SIZE=mipsel-linux-uclibc-size CPPFLAGS="-I$GCC_DIR/include " LDFLAGS="-L$GCC_DIR/lib -L$BUILD_DIR/lib" ./configure --host=mipsel-linux --build=i686-linux



