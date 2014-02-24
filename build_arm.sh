#!/bin/sh


export PATH=/opt/home/arm/4.3.2/bin:$PATH



./configure -C --host=arm-linux

make clean && make

