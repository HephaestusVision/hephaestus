#!/bin/sh

## Example build script for Linux.  Change the paths to appropriate
## locations.  Or just use cmake-gui.

SRC=$(cd $(dirname "$0"); pwd)
BUILD="${SRC}/build"
mkdir -p "$BUILD"
cd "$BUILD"
renice 1 $$
cmake "$SRC" \
    -DVTK_DIR="${HOME}/build/VTK-5-10" \
    -DOpenCV_DIR:PATH="${HOME}/build/OpenCV" \
    -DLIBFREENECT_ROOT:PATH="${HOME}/src/libfreenect" \
    -DLIBFREENECT_BUILD:PATH="${HOME}/build/libfreenect" \
    && make -j4 && echo "SUCCESS!";

