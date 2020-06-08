@echo off
set build=%CD%

set /p ok="Starting c++ driver (press return to continue)..."
cd ../temp/cpp-driver-win32-x64-release-static
xt-cpp-driver.exe true
cd %build%
echo C++ driver finished.

set /p ok="Starting java driver (press return to continue)..."
cd ../temp/java-driver
java -jar com.xt-audio.xt.driver-1.0.6.jar true
cd %build%
echo Java driver finished.
