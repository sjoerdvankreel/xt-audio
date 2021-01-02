@echo off
set /p ok="C++ x86 sample..."
..\dist\cpp\sample\x86\Release\xt-sample.exe
set /p ok="C++ x64 sample..."
..\dist\cpp\sample\x64\Release\xt-sample.exe
set /p ok="Java sample..."
java -jar ..\dist\java\sample\target\xt.sample-1.7.jar
set /p ok=".NET Framework sample..."
..\dist\cli\sample\Release\net48\Xt.Sample.exe
set /p ok=".NET Core sample..."
..\dist\cli\sample\Release\netcoreapp3.1\Xt.Sample.exe