#!/bin/bash
export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

mkdir -p build

if [ "$1" == "--gen" ]; then
    cd build; cmake ../; make; cd .. 

elif [ "$1" == "--gen-extra" ]; then
    cd build; cmake -DCMAKE_EXTRA_FLAGS=ON -DCMAKE_WARNING_FLAGS=ON ../; make; cd ..

elif [ "$1" == "--gen-base" ]; then
    cd build; cmake -DCMAKE_EXTRA_FLAGS=OFF ../; make; cd ..

elif [ "$1" == "--gen-nowarning" ]; then
    cd build; cmake -DCMAKE_WARNING_FLAGS=OFF ../; make; cd ..

elif [ "$1" == "--gen-warning" ]; then
    cd build; cmake -DCMAKE_WARNING_FLAGS=ON ../; make; cd ..

elif [ "$1" == "--make" ]; then
    cd build; make; cd .. 

elif [ "$1" == "--run" ]; then
    
    if [ "$#" -eq 1 ]; then echo "You must provide an binary name to run."
    else cd build; cmake ../; make "$2"; ./bin/"$2" cd .. 
fi

elif [ "$1" == "--test" ]; then
    
    if [ "$#" -eq 1 ]; then echo "You must provide an binary name to run."
    else cd build; cmake ../; make "$2"; ./bin/"$2" --test cd .. 
fi

elif [ "$1" == "--clean" ]; then
    cd build; make clean; cd .. 

elif [ "$1" == "--clean-bin" ]; then
    cd build; rm -rf bin; cd ..

elif [ "$1" == "--clean-all" ]; then
    rm -rf build

elif [ "$1" == "-h"  ] || [ "$1" == "--help" ];  then
   echo "--gen:  Generate make files with CMakeLists.txt and make."
   echo "--make:  run make in 'build/' to build binaries."
   echo "--run <binary_name>:  runs the respective <binary_name> in 'build/bin/'. "

else
  echo "option" "$1" "is invalid."
fi


