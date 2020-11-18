@echo off
setLocal enableDelayedExpansion

REM native
if not exist native (mkdir native)
cd native
cmake .. -G"Visual Studio 16 2019" -DXT_DISABLE_DIRECT_SOUND=%1 -DXT_DISABLE_WASAPI=%2 -DXT_DISABLE_ASIO=%3 -DXT_ASIOSDK_DIR=%4 -DXT_ASMJIT_DIR=%5 > NUL
if !errorlevel! neq 0 exit /b !errorlevel!
msbuild xt-audio.sln /p:Configuration=Debug /verbosity:quiet
if !errorlevel! neq 0 exit /b !errorlevel!
msbuild xt-audio.sln /p:Configuration=Release /verbosity:quiet
if !errorlevel! neq 0 exit /b !errorlevel!
cd ..
copy ..\dist\core\xt\Debug\xt-core.dll ..\dist\cpp\sample\Debug\xt-core.dll
copy ..\dist\core\xt\Release\xt-core.dll ..\dist\cpp\sample\Release\xt-core.dll
if not exist ..\dist\cpp\xt (mkdir ..\dist\cpp\xt)
xcopy ..\src\cpp\xt ..\dist\cpp\xt /s /q /y
if !errorlevel! neq 0 exit /b !errorlevel!
if not exist ..\dist\core\xt\xt\Release\xt (mkdir ..\dist\core\xt\xt\Release\xt)
echo f | xcopy ..\src\core\xt\xt\XtAudio.h ..\dist\core\xt\Release\xt\XtAudio.h /s /q /y /f
if !errorlevel! neq 0 exit /b !errorlevel!

REM java
cd java\xt
call mvn -q install
if !errorlevel! neq 0 exit /b !errorlevel!
cd ..\..
cd java\sample
call mvn -q install
if !errorlevel! neq 0 exit /b !errorlevel!
cd ..\..

REM cli
cd cli
dotnet restore
msbuild Xt.Cli.sln /p:Configuration=Debug /verbosity:quiet
if !errorlevel! neq 0 exit /b !errorlevel!
msbuild Xt.Cli.sln /p:Configuration=Release /verbosity:quiet
if !errorlevel! neq 0 exit /b !errorlevel!
cd ..

REM doc
if not exist ..\dist\cpp\doc (mkdir ..\dist\cpp\doc)
doxygen doc\cpp.doxyfile
if not exist ..\dist\core\doc (mkdir ..\dist\core\doc)
doxygen doc\core.doxyfile
cd java\xt
call mvn -q javadoc:javadoc
cd ..\..
msbuild cli\doc\Xt.Cli.shfbproj /verbosity:quiet