#!/bin/bash

read -p " CHOOSE [c]cmake / [m]make : " input
case $input in
    "c")
        rm -rf build
        cd build
        cmake .. -G "Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=1
        make
        ;;
    "m")
        make clean
        bear -- make # ubuntu20及以下: bear make
        ;;
    *)
        exit
        ;;
esac          
