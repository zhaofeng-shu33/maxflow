#!/bin/bash
set -e -x
mkdir -p build
cd build
cmake ..
make
cp ../test/test.py src/
cp ../example.inp src/
cd src
python3 test.py

