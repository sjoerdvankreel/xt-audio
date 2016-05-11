#!/bin/bash
set -e

# Check necessary environment variables.
: ${XT_ARCH:?"Please set XT_ARCH to x86 or x64."}

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
    cmake ../../build -DCMAKE_BUILD_TYPE=${confs[$C]} -DBUILD_SHARED_LIBS=${types[$L]} && make
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
      FT=../../temp/cpp-driver-linux-$XT_ARCH-${confs[$C]}-${libs[$L]}
      mkdir -p $FT
      cp xt-cpp-driver $FT || :
      FT=../../temp/cpp-sample-linux-$XT_ARCH-${confs[$C]}-${libs[$L]}
      mkdir -p $FT
      cp xt-cpp-sample $FT || :
      FT=../../temp/core-sample-linux-$XT_ARCH-${confs[$C]}-${libs[$L]}
      mkdir -p $FT
      cp xt-core-sample $FT || :
      if [ $C == 1 ]; then
        FD=../../dist/cpp-xt-linux-$XT_ARCH-${libs[$L]}
        mkdir -p $FD
        cp libxt-core.a $FD || :
        cp libxt-cpp.a $FD || :
        FD=../../dist/core-sample-linux-$XT_ARCH-${libs[$L]}
        mkdir -p $FD
        cp xt-core-sample $FD
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
  mkdir -p ../scratch/cli/${confs[$C]}/linux-$XT_ARCH
  cp ../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-shared/libxt-core.so ../scratch/cli/${confs[$C]}/linux-$XT_ARCH
  mkdir -p ../scratch/cli-gui/${confs[$C]}/linux-$XT_ARCH
  cp ../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-shared/libxt-core.so ../scratch/cli-gui/${confs[$C]}/linux-$XT_ARCH
  mkdir -p ../scratch/cli-sample/${confs[$C]}/linux-$XT_ARCH
  cp ../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-shared/libxt-core.so ../scratch/cli-sample/${confs[$C]}/linux-$XT_ARCH
  mkdir -p ../temp/cli-xt-${confs[$C]}/linux-$XT_ARCH
  cp ../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-shared/libxt-core.so ../temp/cli-xt-${confs[$C]}/linux-$XT_ARCH
  mkdir -p ../temp/cli-gui-${confs[$C]}/linux-$XT_ARCH
  cp ../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-shared/libxt-core.so ../temp/cli-gui-${confs[$C]}/linux-$XT_ARCH
  mkdir -p ../temp/cli-gui32-${confs[$C]}/linux-$XT_ARCH
  cp ../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-shared/libxt-core.so ../temp/cli-gui32-${confs[$C]}/linux-$XT_ARCH
  mkdir -p ../temp/cli-sample-${confs[$C]}/linux-$XT_ARCH
  cp ../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-shared/libxt-core.so ../temp/cli-sample-${confs[$C]}/linux-$XT_ARCH
  mkdir -p ../temp/cli-sample32-${confs[$C]}/linux-$XT_ARCH
  cp ../temp/core-xt-linux-$XT_ARCH-${confs[$C]}-shared/libxt-core.so ../temp/cli-sample32-${confs[$C]}/linux-$XT_ARCH
done
mkdir -p ../dist/cli-xt/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../dist/cli-xt/linux-$XT_ARCH
mkdir -p ../dist/cli-gui/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../dist/cli-gui/linux-$XT_ARCH
mkdir -p ../dist/cli-sample/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../dist/cli-sample/linux-$XT_ARCH

# Copy native libraries to java.
mkdir -p ../scratch/java/target/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../scratch/java/target/linux-$XT_ARCH
mkdir -p ../scratch/java-driver/target/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../scratch/java-driver/target/linux-$XT_ARCH
mkdir -p ../scratch/java-sample/target/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../scratch/java-sample/target/linux-$XT_ARCH
mkdir -p ../temp/java-xt/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../temp/java-xt/linux-$XT_ARCH
mkdir -p ../temp/java-driver/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../temp/java-driver/linux-$XT_ARCH
mkdir -p ../temp/java-sample/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../temp/java-sample/linux-$XT_ARCH
mkdir -p ../dist/java-xt/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../dist/java-xt/linux-$XT_ARCH
mkdir -p ../dist/java-sample/linux-$XT_ARCH
cp ../temp/core-xt-linux-$XT_ARCH-release-shared/libxt-core.so ../dist/java-sample/linux-$XT_ARCH
