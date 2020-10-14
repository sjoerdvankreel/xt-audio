#!/bin/bash
set -e

read -p "Starting c++ sample (press return to continue)..."
../dist/cpp-sample-linux-x86-static/xt-cpp-sample
echo C++ sample finished.

read -p "Starting netfx sample (press return to continue)..."
cd ../dist/cli-sample/net48
mono xt-cli-sample.exe
cd ../../../build
echo Netfx sample finished.

read -p "Starting java sample (press return to continue)..."
cd ../dist/java-sample
java -jar com.xt-audio.xt.sample-1.7.jar
cd ../../build
echo Java sample finished.
