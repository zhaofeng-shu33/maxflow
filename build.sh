#!/bin/bash
set -e -x
mkdir -p build
cd build
cmake ..
make
cp ../test/test.py Maxflow/
cp ../example.inp Maxflow/
cd Maxflow
python3 test.py

