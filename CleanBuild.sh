#!/bin/sh
rm -r build
cmake -Bbuild
cd build
make
./cpsc587_a1_hh

