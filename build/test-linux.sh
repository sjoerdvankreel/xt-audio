#!/bin/bash
set -e

echo "C++ debug..."
../dist/cpp/sample/$1/Debug/xt-sample $2
echo $'\n'

echo "C++ release..."
../dist/cpp/sample/$1/Release/xt-sample $2
echo $'\n'

echo "Mono debug..."
mono ../dist/net/sample/Debug/net48/Xt.Sample.exe $2
echo $'\n'

echo "Mono release..."
mono ../dist/net/sample/Release/net48/Xt.Sample.exe $2
echo $'\n'

echo "Java debug..."
java -jar ../dist/java/sample/debug/target/xt.sample.debug-2.2.jar $2
echo $'\n'

echo "Java release..."
java -jar ../dist/java/sample/release/target/xt.sample-2.2.jar $2
echo $'\n'

if [[ "$1" == "x86" ]]
then
  exit 0;
fi

echo "NetCore debug..."
dotnet ../dist/net/sample/Debug/net6.0/Xt.Sample.dll $2
echo $'\n'

echo "NetCore release..."
dotnet ../dist/net/sample/Release/net6.0/Xt.Sample.dll $2
echo $'\n'