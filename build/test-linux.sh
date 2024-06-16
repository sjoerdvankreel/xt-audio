#!/bin/bash
set -e

echo "C++ debug..."
../dist/cpp/sample/Debug/xt-sample $1
echo $'\n'

echo "C++ release..."
../dist/cpp/sample/Release/xt-sample $1
echo $'\n'

echo "Mono debug..."
mono ../dist/net/sample/Debug/net48/Xt.Sample.exe $1
echo $'\n'

echo "Mono release..."
mono ../dist/net/sample/Release/net48/Xt.Sample.exe $1
echo $'\n'

echo "Java debug..."
java -jar ../dist/java/sample/debug/target/xt.sample.debug-2.2.jar $1
echo $'\n'

echo "Java release..."
java -jar ../dist/java/sample/release/target/xt.sample-2.2.jar $1
echo $'\n'

echo "NetCore debug..."
dotnet ../dist/net/sample/Debug/net6.0/Xt.Sample.dll $1
echo $'\n'

echo "NetCore release..."
dotnet ../dist/net/sample/Release/net6.0/Xt.Sample.dll $1
echo $'\n'