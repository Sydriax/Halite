#!/bin/bash

# Install Halite environment
curl "https://halite.io/downloads/environment/HaliteEnvironment-Source.zip" -o "HaliteEnvironment-Source.zip"
mkdir HaliteEnvironment-Source
unzip HaliteEnvironment-Source.zip -d HaliteEnvironment-Source
rm HaliteEnvironment-Source.zip
cd HaliteEnvironment-Source
make
#make install
cd ../
mkdir -p bin
mv HaliteEnvironment-Source/halite ./bin/halite
rm -r HaliteEnvironment-Source 
