# Skype

GUI:

To use the imgui GUI you need to install SDL2 framework:

LINUX - sudo apt-get install libsdl2-2.0 libsdl2-dev

OSX - homebrew install sdl2

###Build shell script

1. To generate the CMake and build it (No extra flags)
```
./build.sh --gen
```
2. To generate the CMake with extra flags
```
./build.sh --gen-extra
```
3. To generate CMake without extra flags
```
./build.sh --gen-base
```
4. To make
```
./build.sh --make
```
5. To run server
```
./build.sh --run server
```
6. To run client
```
./build.sh --run client
```
7. To make clean
```
./build.sh --clean
```
8. To clean binary 
```
./build.sh --clean-bin
```
9. To clean all including the directory `build`
```
./build.sh --clean-all
```
