#!/bin/sh

if [ ! -d "./build" ]; then
  mkdir build
fi

pushd build
CC=clang CXX=clang++ cmake ..
make
popd
