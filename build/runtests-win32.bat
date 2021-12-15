@echo off
set /p ok="C++ x86 sample..."
..\dist\cpp\sample\x86\Release\xt-sample.exe
set /p ok="C++ x64 sample..."
..\dist\cpp\sample\x64\Release\xt-sample.exe
set /p ok="Java sample..."
java -jar ..\dist\java\sample\target\xt.sample-2.0.jar
set /p ok=".NET Framework sample..."
..\dist\net\sample\Release\net48\Xt.Sample.exe
set /p ok=".NET Core sample..."
..\dist\net\sample\Release\net6.0\Xt.Sample.exe