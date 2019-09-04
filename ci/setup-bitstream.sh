#! /bin/bash

BUILDTYPE=release
INSTALL_DIR=$HOME/local

mkdir -p ${INSTALL_DIR}

export LIBRARY_PATH=/usr/lib:/usr/local/lib:$INSTALL_DIR/lib
export LD_LIBRARY_PATH=/usr/lib:/usr/local/lib:$INSTALL_DIR/lib

export CC=gcc-9
export CXX=g++-9

meson build -Dbuildtype=$BUILDTYPE --prefix=$INSTALL_DIR
cd build
ninja -v
ninja install -v

ninja tidy
