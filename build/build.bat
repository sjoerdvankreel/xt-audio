@echo off
setLocal enableDelayedExpansion

set archs[0]=x86
set archs[1]=x64
set vsarchs[0]=Win32
set vsarchs[1]=x64

REM native binaries
for /L %%A in (0, 1, 1) do (
  if not exist native\win32\!archs[%%A]! (mkdir native\win32\!archs[%%A]!)
  cd native\win32\!archs[%%A]!
  cmake ..\..\.. -G"Visual Studio 17 2022" -A !vsarchs[%%A]! -DXT_ARCH=!archs[%%A]! -DXT_ENABLE_ALSA=0 -DXT_ENABLE_JACK=0 -DXT_ENABLE_PULSE=0 -DXT_ENABLE_DSOUND=%1 -DXT_ENABLE_WASAPI=%2 -DXT_ENABLE_ASIO=%3 -DXT_ASIOSDK_DIR=%4 -DXT_ASMJIT_DIR=%5 > NUL
  if !errorlevel! neq 0 exit /b !errorlevel!
  msbuild xt-audio.sln /p:Configuration=Debug /verbosity:quiet
  if !errorlevel! neq 0 exit /b !errorlevel!
  msbuild xt-audio.sln /p:Configuration=Release /verbosity:quiet
  if !errorlevel! neq 0 exit /b !errorlevel!
  cd ..\..\..
  copy ..\dist\core\xt\!archs[%%A]!\Debug\xt-audio.dll ..\dist\cpp\sample\!archs[%%A]!\Debug\xt-audio.dll
  copy ..\dist\core\xt\!archs[%%A]!\Debug\xt-audio.pdb ..\dist\cpp\sample\!archs[%%A]!\Debug\xt-audio.pdb
  copy ..\dist\core\xt\!archs[%%A]!\Release\xt-audio.dll ..\dist\cpp\sample\!archs[%%A]!\Release\xt-audio.dll
)

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
copy pom.xml ..\..\..\dist\java\xt\release\target\xt.audio-2.1.pom
call mvn -f pom.debug.xml -q install
if !errorlevel! neq 0 exit /b !errorlevel!
copy pom.debug.xml ..\..\..\dist\java\xt\debug\target\xt.audio.debug-2.1.pom
cd ..\..
cd java\sample
call mvn -q install
if !errorlevel! neq 0 exit /b !errorlevel!
call mvn -f pom.debug.xml -q install
if !errorlevel! neq 0 exit /b !errorlevel!
cd ..\..

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