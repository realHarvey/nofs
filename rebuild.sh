#!/bin/bash

read -p " CHOOSE [c]cmake / [m]make : " input
case $input in
    "c")
        cd build
        rm -rf * *.*
        cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=1
        make
        ;;
    "m")
        make clean
        bear -- make
        ;;
    *)
        exit
        ;;
esac          
