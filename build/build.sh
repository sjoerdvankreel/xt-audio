#!/bin/bash
set -e

mkdir -p native/linux/debug
cd native/linux/debug
cmake ../../.. -DCMAKE_BUILD_TYPE=Debug -DXT_ENABLE_ASIO=0 -DXT_ENABLE_WASAPI=0 -DXT_ENABLE_DSOUND=0 -DXT_ENABLE_PULSE="$1" -DXT_ENABLE_ALSA="$2" -DXT_ENABLE_JACK="$3" >/dev/null
make >/dev/null
cd ../../..
cp ../dist/core/xt/Debug/libxt-audio.so ../dist/cpp/sample/Debug/libxt-audio.so || :

mkdir -p native/linux/release
cd native/linux/release
cmake ../../.. -DCMAKE_BUILD_TYPE=Release -DXT_ENABLE_ASIO=0 -DXT_ENABLE_WASAPI=0 -DXT_ENABLE_DSOUND=0 -DXT_ENABLE_PULSE="$1" -DXT_ENABLE_ALSA="$2" -DXT_ENABLE_JACK="$3" >/dev/null
make >/dev/null
cd ../../..
cp ../dist/core/xt/Release/libxt-audio.so ../dist/cpp/sample/Release/libxt-audio.so || :