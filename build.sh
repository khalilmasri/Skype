#!/bin/bash

mkdir -p build

if [ "$1" == "--gen" ]; then
    cd build; cmake ../; make; cd .. 

elif [ "$1" == "--make" ]; then
    cd build; make; cd .. 


elif [ "$1" == "--run" ]; then
    
    if [ "$#" -eq 1 ]; then echo "You must provide an binary name to run."
    else cd build; cmake ../; make; ./bin/"$2" cd .. 
    fi

elif [ "$1" == "--run-client" ]; then
    cd build; make; ./bin/ cd .. 

elif [ "$1" == "-h"  ] || [ "$1" == "--help" ];  then
   echo "--gen:  Generate make files with CMakeLists.txt and make."
   echo "--make:  run make in 'build/' to build binaries."
   echo "--run <binary_name>:  runs the respective <binary_name> in 'build/bin/'. "

else
  echo "option" "$1" "is invalid."
fi


