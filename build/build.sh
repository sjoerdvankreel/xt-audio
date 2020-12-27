#!/bin/bash
set -e

mkdir -p native/linux/"$1"/debug
cd native/linux/"$1"/debug
cmake ../../../.. -DCMAKE_BUILD_TYPE=Debug -DXT_ARCH="$1" -DXT_ENABLE_ASIO=0 -DXT_ENABLE_WASAPI=0 -DXT_ENABLE_DSOUND=0 -DXT_ENABLE_PULSE="$2" -DXT_ENABLE_ALSA="$3" -DXT_ENABLE_JACK="$4" >/dev/null
make >/dev/null
cd ../../../..

mkdir -p native/linux/"$1"/release
cd native/linux/"$1"/release
cmake ../../../.. -DCMAKE_BUILD_TYPE=Release -DXT_ARCH="$1" -DXT_ENABLE_ASIO=0 -DXT_ENABLE_WASAPI=0 -DXT_ENABLE_DSOUND=0 -DXT_ENABLE_PULSE="$2" -DXT_ENABLE_ALSA="$3" -DXT_ENABLE_JACK="$4" >/dev/null
make >/dev/null
cd ../../../..

cp ../dist/core/xt/Debug/libxt-core.so ../dist/cpp/sample/Debug/libxt-core.so || :
cp ../dist/core/xt/Release/libxt-core.so ../dist/cpp/sample/Release/libxt-core.so || :