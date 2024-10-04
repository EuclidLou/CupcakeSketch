#! /bin/bash

cd ~/Sketch/
make clean
make all
./exec/exp > runing_output.log
