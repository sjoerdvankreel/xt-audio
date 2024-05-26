@echo off

echo C++ debug...
..\dist\cpp\sample\%1\Debug\xt-sample.exe "%2"
if %errorlevel% neq 0 exit /b !errorlevel!
echo:

echo C++ release...
..\dist\cpp\sample\%1\Release\xt-sample.exe "%2"
if %errorlevel% neq 0 exit /b !errorlevel!
echo:

echo NetFX debug...
..\dist\net\sample\Debug\net48\Xt.Sample.exe "%2"
if %errorlevel% neq 0 exit /b !errorlevel!
echo:

echo NetFX release...
..\dist\net\sample\Release\net48\Xt.Sample.exe "%2"
if %errorlevel% neq 0 exit /b !errorlevel!
echo:

echo NetCore debug...
dotnet ..\dist\net\sample\Debug\net6.0\Xt.Sample.dll "%2"
if %errorlevel% neq 0 exit /b !errorlevel!
echo:

echo NetCore release...
dotnet ..\dist\net\sample\Release\net6.0\Xt.Sample.dll "%2"
if %errorlevel% neq 0 exit /b !errorlevel!
echo:

echo Java debug...
java -jar ..\dist\java\sample\debug\target\xt.sample.debug-2.1.jar "%2"
if %errorlevel% neq 0 exit /b !errorlevel!
echo:

echo Java release...
java -jar ..\dist\java\sample\release\target\xt.sample-2.1.jar "%2"
if %errorlevel% neq 0 exit /b !errorlevel!
echo: