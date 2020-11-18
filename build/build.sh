#!/bin/bash
set -e

# native
mkdir -p native/linux/debug
cd native/linux/debug
cmake ../../.. -DCMAKE_BUILD_TYPE=Debug -DXT_ENABLE_PULSE_AUDIO="$1" -DXT_ENABLE_ALSA="$2" -DXT_ENABLE_JACK="$3" >/dev/null
make >/dev/null
cd ../../..
mkdir -p native/linux/release
cd native/linux/release
cmake ../../.. -DCMAKE_BUILD_TYPE=Release -DXT_ENABLE_PULSE_AUDIO="$1" -DXT_ENABLE_ALSA="$2" -DXT_ENABLE_JACK="$3" >/dev/null
make >/dev/null
cd ../../..

exit

# Check necessary environment variables.
: ${XT_ARCH:?"Please set XT_ARCH to x86 or x64."}

# Selective compilation support.
# Command line args are (in order):
# disable-pulse (OFF/ON), disable-alsa (OFF/ON), disable-jack (OFF/ON).
if [ $# -ne 3 ]; then
  echo "Please specify which backends to compile."
  exit 1
fi

disable_pulse=OFF
if [ "$1" == "ON" ]; then
  disable_pulse=ON
  echo Building without Pulse support.
else
  echo Building with Pulse support.
fi

disable_alsa=OFF
if [ "$2" == "ON" ]; then
  disable_alsa=ON
  echo Building without ALSA support.
else
  echo Building with ALSA support.
fi

disable_jack=OFF
if [ "$3" == "ON" ]; then
  disable_jack=ON
  echo Building without JACK support.
else
  echo Building with JACK support.
fi

read -p "Continue (y/n)? " CONT
if [ "$CONT" != "y" ]; then
  exit 0
fi

# Intermediate folders.
# Scratch is build directory.
# Temp is full output (all configurations).
# Dist is only what get's released.
mkdir -p ../temp
mkdir -p ../dist
mkdir -p ../scratch

# Native project params.
types[0]=OFF
types[1]=ON
libs[0]=static
libs[1]=shared
confs[0]=debug
confs[1]=release

# Build native projects.
# For core library, all combinations of debug/release, shared/static.
# For all others, shared versions are not supported.
cd ../scratch
for L in 0 1
do
  for C in 0 1
  do
    FS=linux-$XT_ARCH-${confs[$C]}-${libs[$L]}
    mkdir -p $FS && cd $FS
    cmake ../../build -DCMAKE_BUILD_TYPE=${confs[$C]} -DDISABLE_PULSE=${disable_pulse} -DDISABLE_ALSA=${disable_alsa} -DDISABLE_JACK=${disable_jack} -DBUILD_SHARED_LIBS=${types[$L]} && make
    FT=../../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-${libs[$L]}
    mkdir -p $FT
    cp libxt-core.a $FT || :
    cp libxt-core.so $FT || :
    if [ $C == 1 ]; then
      FD=../../dist/core-xt-linux-$XT_ARCH-${libs[$L]}
      mkdir -p $FD
      cp libxt-core.so $FD || :
      cp libxt-core.a $FD || :
    fi
    if [ $L == 0 ]; then
      FT=../../temp/cpp-xt-linux-$XT_ARCH-${confs[$C]}-${libs[$L]}
      mkdir -p $FT
      cp libxt-core.a $FT || :
      cp libxt-cpp.a $FT || :
      FT=../../temp/cpp-sample-linux-$XT_ARCH-${confs[$C]}-${libs[$L]}
      mkdir -p $FT
      cp xt-cpp-sample $FT || :
      if [ $C == 1 ]; then
        FD=../../dist/cpp-xt-linux-$XT_ARCH-${libs[$L]}
        mkdir -p $FD
        cp libxt-core.a $FD || :
        cp libxt-cpp.a $FD || :
        FD=../../dist/cpp-sample-linux-$XT_ARCH-${libs[$L]}
        mkdir -p $FD
        cp xt-cpp-sample $FD
      fi
    fi
    cd ../
  done
done
cd ../build

# Copy native libraries to cli.
for C in 0 1
do
  mkdir -p ../scratch/cli/${confs[$C]}/net48/linux-$XT_ARCH
  cp ../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-shared/libxt-core.so ../scratch/cli/${confs[$C]}/net48/linux-$XT_ARCH
  mkdir -p ../scratch/cli/${confs[$C]}/netcoreapp3.1/linux-$XT_ARCH
  cp ../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-shared/libxt-core.so ../scratch/cli/${confs[$C]}/netcoreapp3.1/linux-$XT_ARCH
  mkdir -p ../scratch/cli-gui/${confs[$C]}/net48/linux-$XT_ARCH
  cp ../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-shared/libxt-core.so ../scratch/cli-gui/${confs[$C]}/net48/linux-$XT_ARCH
  mkdir -p ../scratch/cli-gui/${confs[$C]}/netcoreapp3.1/linux-$XT_ARCH
  cp ../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-shared/libxt-core.so ../scratch/cli-gui/${confs[$C]}/netcoreapp3.1/linux-$XT_ARCH
  mkdir -p ../scratch/cli-sample/${confs[$C]}/net48/linux-$XT_ARCH
  cp ../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-shared/libxt-core.so ../scratch/cli-sample/${confs[$C]}/net48/linux-$XT_ARCH
  mkdir -p ../scratch/cli-sample/${confs[$C]}/netcoreapp3.1/linux-$XT_ARCH
  cp ../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-shared/libxt-core.so ../scratch/cli-sample/${confs[$C]}/netcoreapp3.1/linux-$XT_ARCH
  mkdir -p ../temp/cli-xt-${confs[$C]}/net48/linux-$XT_ARCH
  cp ../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-shared/libxt-core.so ../temp/cli-xt-${confs[$C]}/net48/linux-$XT_ARCH
  mkdir -p ../temp/cli-xt-${confs[$C]}/netcoreapp3.1/linux-$XT_ARCH
  cp ../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-shared/libxt-core.so ../temp/cli-xt-${confs[$C]}/netcoreapp3.1/linux-$XT_ARCH
  mkdir -p ../temp/cli-gui-${confs[$C]}/net48/linux-$XT_ARCH
  cp ../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-shared/libxt-core.so ../temp/cli-gui-${confs[$C]}/net48/linux-$XT_ARCH
  mkdir -p ../temp/cli-gui-${confs[$C]}/netcoreapp3.1/linux-$XT_ARCH
  cp ../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-shared/libxt-core.so ../temp/cli-gui-${confs[$C]}/netcoreapp3.1/linux-$XT_ARCH
  mkdir -p ../temp/cli-gui32-${confs[$C]}/net48/linux-$XT_ARCH
  cp ../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-shared/libxt-core.so ../temp/cli-gui32-${confs[$C]}/net48/linux-$XT_ARCH
  mkdir -p ../temp/cli-sample-${confs[$C]}/net48/linux-$XT_ARCH
  cp ../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-shared/libxt-core.so ../temp/cli-sample-${confs[$C]}/net48/linux-$XT_ARCH
  mkdir -p ../temp/cli-sample-${confs[$C]}/netcoreapp3.1/linux-$XT_ARCH
  cp ../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-shared/libxt-core.so ../temp/cli-sample-${confs[$C]}/netcoreapp3.1/linux-$XT_ARCH
  mkdir -p ../temp/cli-sample32-${confs[$C]}/net48/linux-$XT_ARCH
  cp ../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-shared/libxt-core.so ../temp/cli-sample32-${confs[$C]}/net48/linux-$XT_ARCH
done
mkdir -p ../dist/cli-xt/net48/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../dist/cli-xt/net48/linux-$XT_ARCH
mkdir -p ../dist/cli-xt/netcoreapp3.1/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../dist/cli-xt/netcoreapp3.1/linux-$XT_ARCH
mkdir -p ../dist/cli-gui/net48/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../dist/cli-gui/net48/linux-$XT_ARCH
mkdir -p ../dist/cli-gui/netcoreapp3.1/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../dist/cli-gui/netcoreapp3.1/linux-$XT_ARCH
mkdir -p ../dist/cli-sample/net48/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../dist/cli-sample/net48/linux-$XT_ARCH
mkdir -p ../dist/cli-sample/netcoreapp3.1/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../dist/cli-sample/netcoreapp3.1/linux-$XT_ARCH

# Copy native libraries to java.
mkdir -p ../scratch/java/target/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../scratch/java/target/linux-$XT_ARCH
mkdir -p ../scratch/java-sample/target/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../scratch/java-sample/target/linux-$XT_ARCH
mkdir -p ../temp/java-xt/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../temp/java-xt/linux-$XT_ARCH
mkdir -p ../temp/java-sample/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../temp/java-sample/linux-$XT_ARCH
mkdir -p ../dist/java-xt/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../dist/java-xt/linux-$XT_ARCH
mkdir -p ../dist/java-sample/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../dist/java-sample/linux-$XT_ARCH
