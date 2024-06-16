@echo off
setLocal enableDelayedExpansion

REM native binaries
if not exist native\win32 (mkdir native\win32)
cd native\win32
cmake ..\.. -G"Visual Studio 17 2022" -A x64 -DXT_ENABLE_ALSA=0 -DXT_ENABLE_JACK=0 -DXT_ENABLE_PULSE=0 -DXT_ENABLE_DSOUND=%1 -DXT_ENABLE_WASAPI=%2 -DXT_ENABLE_ASIO=%3 -DXT_ASIOSDK_DIR=%4 -DXT_ASMJIT_DIR=%5
if !errorlevel! neq 0 exit /b !errorlevel!
msbuild xt-audio.sln /p:Configuration=Debug
if !errorlevel! neq 0 exit /b !errorlevel!
msbuild xt-audio.sln /p:Configuration=Release
if !errorlevel! neq 0 exit /b !errorlevel!
cd ..\..
copy ..\dist\core\xt\Debug\xt-audio.dll ..\dist\cpp\sample\Debug\xt-audio.dll
copy ..\dist\core\xt\Debug\xt-audio.pdb ..\dist\cpp\sample\Debug\xt-audio.pdb
copy ..\dist\core\xt\Release\xt-audio.dll ..\dist\cpp\sample\Release\xt-audio.dll

REM native includes
if not exist ..\dist\cpp\xt\include (mkdir ..\dist\cpp\xt\include)
xcopy ..\src\cpp\xt ..\dist\cpp\xt\include /s /q /y
if !errorlevel! neq 0 exit /b !errorlevel!
if not exist ..\dist\core\xt\include (mkdir ..\dist\core\xt\include)
echo d | xcopy ..\src\core\xt\xt\api\*.h ..\dist\core\xt\include\xt\api /s /q /y
echo f | xcopy ..\src\core\xt\xt\XtAudio.h ..\dist\core\xt\include\xt\XtAudio.h /q /y /f
if !errorlevel! neq 0 exit /b !errorlevel!

REM java
cd java\xt
call mvn -q install
if !errorlevel! neq 0 exit /b !errorlevel!
copy pom.xml ..\..\..\dist\java\xt\release\target\xt.audio-2.2.pom
call mvn -f pom.debug.xml -q install
if !errorlevel! neq 0 exit /b !errorlevel!
copy pom.debug.xml ..\..\..\dist\java\xt\debug\target\xt.audio.debug-2.2.pom
cd ..\..
cd java\sample
call mvn -q install
if !errorlevel! neq 0 exit /b !errorlevel!
call mvn -f pom.debug.xml -q install
if !errorlevel! neq 0 exit /b !errorlevel!
cd ..\..

goto :EOF

REM net
cd net
dotnet restore
msbuild Xt.Audio.sln /p:Configuration=Debug /verbosity:quiet
if !errorlevel! neq 0 exit /b !errorlevel!
msbuild Xt.Audio.sln /p:Configuration=Release /verbosity:quiet
if !errorlevel! neq 0 exit /b !errorlevel!
cd ..

REM doc
if not exist ..\dist\cpp\doc (mkdir ..\dist\cpp\doc)
doxygen native\doc\cpp.doxyfile
if not exist ..\dist\core\doc (mkdir ..\dist\core\doc)
doxygen native\doc\core.doxyfile
cd java\xt
call mvn -q javadoc:javadoc
cd ..\..
msbuild net\doc\Xt.Audio.shfbproj /verbosity:quiet

REM package
cd ..
tar.exe -cf xt-audio.zip -T dist-files.txt
cd build

REM package nuget
cd ..
nuget pack Xt.Audio.nuspec
cd build