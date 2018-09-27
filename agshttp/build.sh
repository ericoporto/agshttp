#!/bin/bash
#tested in ubuntu 16.04

g++ -std=c++0x -shared -o libagshttp.so -fPIC  agshttp.cpp 
#g++ -lSDL2 -shared -o libagshttp.so -fPIC -m32 agshttp.cpp 
