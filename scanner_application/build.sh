#!/bin/sh

## Example build script for Linux.  Change the paths to appropriate
## locations.  Or just use cmake-gui.

SRC=$(cd $(dirname "$0"); pwd)
BUILD="${SRC}/build"
mkdir -p "$BUILD"
cd "$BUILD"
cmake "$SRC" \
    -DVTK_DIR="${HOME}/build/VTK-5-10" \
    -DOpenCV_DIR:PATH="${HOME}/build/OpenCV" \
    -DLIB_FREENECT_SRC:PATH="${HOME}/src/libfreenect" \
    -DLIB_FREENECT_BUILD:PATH="${HOME}/build/libfreenect" \
    && make && echo "SUCCESS!";

