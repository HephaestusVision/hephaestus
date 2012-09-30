#!/bin/sh

OpenCV_DIR="/home/hal/build/OpenCV"
VTK_DIR="/home/hal/build/VTK-5-10"
LIB_FREENECT_SRC="/home/hal/src/libfreenect"
LIB_FREENECT_BUILD="/home/hal/build/libfreenect"

export VTK_DIR
export OpenCV_DIR
export LIB_FREENECT_SRC
export LIB_FREENECT_BUILD

SRC=$(cd $(dirname "$0"); pwd)
BUILD="${SRC}/build"
mkdir -p "$BUILD"
cd "$BUILD"
cmake "$SRC" && make && echo "SUCCESS!";
