@echo off
set build=%CD%

set /p ok="Starting c++ sample (press return to continue)..."
cd ../dist/cpp-sample-win32-x64-static
xt-cpp-sample.exe
cd %build%
echo C++ sample finished.

set /p ok="Starting netfx sample (press return to continue)..."
cd ../dist/cli-sample/net48
xt-cli-sample.exe
cd %build%
echo Netfx sample finished.

set /p ok="Starting netcore sample (press return to continue)..."
cd ../dist/cli-sample/netcoreapp3.1
xt-cli-sample.exe
cd %build%
echo Netcore sample finished.

set /p ok="Starting java sample (press return to continue)..."
cd ../dist/java-sample
java -jar com.xt-audio.xt.sample-1.0.6.jar
cd %build%
echo Java sample finished.
