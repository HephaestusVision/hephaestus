#!/bin/sh
SRC=$(cd $(dirname "$0"); pwd)
BUILD="${SRC}/build"
mkdir -p "$BUILD"
cd "$BUILD"
cmake "$SRC" && make && echo "SUCCESS!";
