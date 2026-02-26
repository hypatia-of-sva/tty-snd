#!/bin/bash
cd src
cmake -S . -B ../build/linux
cmake --build ../build/linux -j $(nproc)
