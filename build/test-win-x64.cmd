@echo off

echo C++ debug...
..\dist\cpp\sample\x64\Debug\xt-sample.exe "%1"
if %errorlevel% neq 0 exit /b !errorlevel!
echo:

echo C++ release...
..\dist\cpp\sample\x64\Release\xt-sample.exe "%1"
if %errorlevel% neq 0 exit /b !errorlevel!
echo:

echo NetFX debug...
..\dist\net\sample\Debug\net48\Xt.Sample.exe "%1"
if %errorlevel% neq 0 exit /b !errorlevel!
echo:

echo NetFX release...
..\dist\net\sample\Release\net48\Xt.Sample.exe "%1"
if %errorlevel% neq 0 exit /b !errorlevel!
echo:

echo NetCore debug...
..\dist\net\sample\Debug\net6.0\Xt.Sample.exe "%1"
if %errorlevel% neq 0 exit /b !errorlevel!
echo:

echo NetCore release...
..\dist\net\sample\Release\net6.0\Xt.Sample.exe "%1"
if %errorlevel% neq 0 exit /b !errorlevel!
echo:

echo Java debug...
java -jar ..\dist\java\sample\debug\target\xt.sample.debug-2.0-SNAPSHOT.jar "%1"
if %errorlevel% neq 0 exit /b !errorlevel!
echo:

echo Java release...
java -jar ..\dist\java\sample\release\target\xt.sample-2.0-SNAPSHOT.jar "%1"
if %errorlevel% neq 0 exit /b !errorlevel!
echo: