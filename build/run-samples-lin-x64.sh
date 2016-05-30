#!/bin/bash
set -e

../dist/cpp-sample-linux-x64-static/xt-cpp-sample
cd ../dist/cli-sample/
mono xt-cli-sample.exe
cd ../../build
cd ../dist/java-sample
java -jar com.xt-audio.xt.sample-1.0.2.jar
cd ../../build
