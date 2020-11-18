@echo off
setLocal enableDelayedExpansion

REM native
if not exist native (mkdir native)
cd native
cmake .. -G"Visual Studio 16 2019" -DDISABLE_DIRECT_SOUND=%1 -DDISABLE_WASAPI=%2 -DDISABLE_ASIO=%3 -DXT_ASIOSDK_DIR=%4 -DXT_ASMJIT_DIR=%5 > NUL
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

exit /B


REM Selective compilation support.
REM Command line args are (in order): 
REM disable-dsound (OFF/ON), disable-wasapi (OFF/ON), disable-asio (OFF/ON), 
REM path-to-asio-source (required for asio), path-to-asmjit-source (required for asio).

IF "%3"=="" (
  echo Please specify which backends to compile.
  exit /b 1  
)

set disable_dsound=OFF
IF "%1"=="ON" (
  set disable_dsound=ON
  echo Building without DSound support.
) else (
  echo Building with DSound support.
)

set disable_wasapi=OFF
IF "%2"=="ON" (
  set disable_wasapi=ON
  echo Building without WASAPI support.
) else (
  echo Building with WASAPI support.
)

set disable_asio=OFF
IF "%3"=="ON" (
  set disable_asio=ON
  echo Building without ASIO support.
) else (
  echo Building with ASIO support.
)

if "%disable_asio%"=="OFF" (
  IF "%5"=="" (
    echo Please specify the paths to the asio sdk and asmjit sources.
    exit /b 1
  )
)

set /p ok="Continue (y/n)? "
IF NOT "%ok%"=="y" (
  exit /b 0
)

REM Intermediate folders.
REM Scratch is build directory.
REM Temp is full output (all configurations).
REM Dist is only what get's released.
if not exist "..\temp" (mkdir "..\temp")
if not exist "..\dist" (mkdir "..\dist")
if not exist "..\scratch" (mkdir "..\scratch")

REM Native project params.
set confs[0]=debug
set confs[1]=release
set generator="Visual Studio 16 2019"

REM Build native projects.
cd ..\scratch
if not exist win32 (mkdir win32)
cd win32
cmake ..\..\build -G %generator% -DDISABLE_DSOUND=%disable_dsound% -DDISABLE_WASAPI=%disable_wasapi% -DDISABLE_ASIO=%disable_asio% -DXT_ASIOSDK_DIR=%4 -DXT_ASMJIT_DIR=%5
if !errorlevel! neq 0 exit /b !errorlevel!
for /L %%C in (0, 1, 1) do (
  msbuild xt-audio.sln /p:Configuration=!confs[%%C]!
  if !errorlevel! neq 0 exit /b !errorlevel!
  set FT=..\..\temp\core-xt-win32-!confs[%%C]!
  if not exist !FT! (mkdir !FT!)
  copy !confs[%%C]!\xt-core.dll !FT!\xt-core.dll
  if %%C == 0 (
    copy !confs[%%C]!\xt-core.pdb !FT!\xt-core.pdb
  )
  if %%C == 1 (
    set FD=..\..\dist\core-xt-win32
    if not exist !FD! (mkdir !FD!)
    copy !FT! !FD!
  )
  set FT=..\..\temp\cpp-sample-win32-!confs[%%C]!
  if not exist !FT! (mkdir !FT!)
  copy !confs[%%C]!\xt-core.dll !FT!\xt-core.dll
  if %%C == 0 (
    copy !confs[%%C]!\xt-core.pdb !FT!\xt-core.pdb
  )
  if %%C == 1 (
    set FD=..\..\dist\cpp-sample-win32
    if not exist !FD! (mkdir !FD!)
    copy !FT! !FD!
  )
  set FT=..\..\temp\cpp-sample-win32-!confs[%%C]!
  if not exist !FT! (mkdir !FT!)
  copy !confs[%%C]!\xt-cpp-sample.exe !FT!\xt-cpp-sample.exe
  if %%C == 0 (
    copy !confs[%%C]!\xt-cpp-sample.pdb !FT!\xt-cpp-sample.pdb
  )
  if %%C == 1 (
    set FD=..\..\dist\cpp-sample-win32
    if not exist !FD! (mkdir !FD!)
    copy !FT! !FD!
  )
)
cd ..

cd ..\build

REM build cli projects.
dotnet restore
for %%C in (debug release) do (
  msbuild cli.sln /p:Configuration=%%C
  if !errorlevel! neq 0 exit /b !errorlevel!
  if not exist ..\scratch\cli\%%C\net48 mkdir ..\scratch\cli\%%C\net48
  copy ..\temp\core-xt-win32-%%C\*.* ..\scratch\cli\%%C\net48
  if not exist ..\scratch\cli\%%C\netcoreapp3.1 mkdir ..\scratch\cli\%%C\netcoreapp3.1
  copy ..\temp\core-xt-win32-%%C\*.* ..\scratch\cli\%%C\netcoreapp3.1
  if not exist ..\scratch\cli-gui\%%C\net48\ mkdir ..\scratch\cli-gui\%%C\net48
  copy ..\temp\core-xt-win32-%%C\*.* ..\scratch\cli-gui\%%C\net48
  if not exist ..\scratch\cli-gui\%%C\netcoreapp3.1 mkdir ..\scratch\cli-gui\%%C\netcoreapp3.1
  copy ..\temp\core-xt-win32-%%C\*.* ..\scratch\cli-gui\%%C\netcoreapp3.1
  if not exist ..\scratch\cli-sample\%%C\net48 mkdir ..\scratch\cli-sample\%%C\net48
  copy ..\temp\core-xt-win32-%%C\*.* ..\scratch\cli-sample\%%C\net48
  if not exist ..\scratch\cli-sample\%%C\netcoreapp3.1 mkdir ..\scratch\cli-sample\%%C\netcoreapp3.1
  copy ..\temp\core-xt-win32-%%C\*.* ..\scratch\cli-sample\%%C\netcoreapp3.1
  if not exist ..\temp\cli-xt-%%C\net48 (mkdir ..\temp\cli-xt-%%C\net48)
  xcopy /y /s ..\scratch\cli\%%C\net48 ..\temp\cli-xt-%%C\net48
  if not exist ..\temp\cli-xt-%%C\netcoreapp3.1 (mkdir ..\temp\cli-xt-%%C\netcoreapp3.1)
  xcopy /y /s ..\scratch\cli\%%C\netcoreapp3.1 ..\temp\cli-xt-%%C\netcoreapp3.1
  if not exist ..\temp\cli-gui-%%C\net48 (mkdir ..\temp\cli-gui-%%C\net48)
  xcopy /y /s ..\scratch\cli-gui\%%C\net48 ..\temp\cli-gui-%%C\net48
  if not exist ..\temp\cli-gui-%%C\netcoreapp3.1 (mkdir ..\temp\cli-gui-%%C\netcoreapp3.1)
  xcopy /y /s ..\scratch\cli-gui\%%C\netcoreapp3.1 ..\temp\cli-gui-%%C\netcoreapp3.1
  copy ..\scratch\cli\%%C\net48\xt-cli.dll.config ..\scratch\cli-gui\%%C\net48\xt-cli.dll.config
  copy ..\scratch\cli\%%C\netcoreapp3.1\xt-cli.dll.config ..\scratch\cli-gui\%%C\netcoreapp3.1\xt-cli.dll.config
  copy ..\scratch\cli\%%C\net48\xt-cli.dll.config ..\temp\cli-gui-%%C\net48\xt-cli.dll.config
  copy ..\scratch\cli\%%C\netcoreapp3.1\xt-cli.dll.config ..\temp\cli-gui-%%C\netcoreapp3.1\xt-cli.dll.config
  if not exist ..\temp\cli-sample-%%C\net48 (mkdir ..\temp\cli-sample-%%C\net48)
  xcopy /y /s ..\scratch\cli-sample\%%C\net48 ..\temp\cli-sample-%%C\net48
  if not exist ..\temp\cli-sample-%%C\netcoreapp3.1 (mkdir ..\temp\cli-sample-%%C\netcoreapp3.1)
  xcopy /y /s ..\scratch\cli-sample\%%C\netcoreapp3.1 ..\temp\cli-sample-%%C\netcoreapp3.1
  if !errorlevel! neq 0 exit /b !errorlevel!
  copy ..\scratch\cli\%%C\net48\xt-cli.dll.config ..\scratch\cli-sample\%%C\net48\xt-cli.dll.config
  copy ..\scratch\cli\%%C\netcoreapp3.1\xt-cli.dll.config ..\scratch\cli-sample\%%C\netcoreapp3.1\xt-cli.dll.config
  copy ..\scratch\cli\%%C\net48\xt-cli.dll.config ..\temp\cli-sample-%%C\net48\xt-cli.dll.config
  copy ..\scratch\cli\%%C\netcoreapp3.1\xt-cli.dll.config ..\temp\cli-sample-%%C\netcoreapp3.1\xt-cli.dll.config
)
if not exist ..\dist\cli-xt\net48 (mkdir ..\dist\cli-xt\net48)
if not exist ..\dist\cli-xt\netcoreapp3.1 (mkdir ..\dist\cli-xt\netcoreapp3.1)
if not exist ..\dist\cli-gui\net48 (mkdir ..\dist\cli-gui\net48)
if not exist ..\dist\cli-gui\netcoreapp3.1 (mkdir ..\dist\cli-gui\netcoreapp3.1)
if not exist ..\dist\cli-sample\net48 (mkdir ..\dist\cli-sample\net48)
if not exist ..\dist\cli-sample\netcoreapp3.1 (mkdir ..\dist\cli-sample\netcoreapp3.1)
xcopy /y /s ..\temp\cli-xt-release\net48 ..\dist\cli-xt\net48
xcopy /y /s ..\temp\cli-xt-release\netcoreapp3.1 ..\dist\cli-xt\netcoreapp3.1
xcopy /y /s ..\temp\cli-gui-release\net48 ..\dist\cli-gui\net48
xcopy /y /s ..\temp\cli-gui-release\netcoreapp3.1 ..\dist\cli-gui\netcoreapp3.1
xcopy /y /s ..\temp\cli-sample-release\net48 ..\dist\cli-sample\net48
xcopy /y /s ..\temp\cli-sample-release\netcoreapp3.1 ..\dist\cli-sample\netcoreapp3.1

REM build java projects.
cd java
call mvn install
if !errorlevel! neq 0 exit /b !errorlevel!
cd ..
if not exist ..\temp\java-xt (mkdir ..\temp\java-xt)
if not exist ..\scratch\java\target (mkdir ..\scratch\java\target)
copy ..\scratch\java\target\*.jar ..\temp\java-xt
copy ..\temp\core-xt-win32-release\xt-core.dll ..\temp\java-xt
copy ..\temp\core-xt-win32-release\xt-core.dll ..\scratch\java\target
if not exist ..\dist\java-xt (mkdir ..\dist\java-xt)
xcopy /y /s ..\temp\java-xt\*.* ..\dist\java-xt\*.*

cd java-sample
call mvn install
if !errorlevel! neq 0 exit /b !errorlevel!
cd ..
if not exist ..\temp\java-sample (mkdir ..\temp\java-sample)
if not exist ..\scratch\java-sample\target (mkdir ..\scratch\java-sample\target)
copy ..\scratch\java-sample\target\*.jar ..\temp\java-sample
copy ..\temp\core-xt-win32-release\xt-core.dll ..\temp\java-sample
copy ..\temp\core-xt-win32-release\xt-core.dll ..\scratch\java-sample\target
if not exist ..\dist\java-sample (mkdir ..\dist\java-sample)
xcopy /y /s ..\temp\java-sample\*.* ..\dist\java-sample\*.*

REM build native documentation.
if not exist "..\dist\doc\cpp" (mkdir "..\dist\doc\cpp")
doxygen cpp.doxyfile
if not exist "..\dist\doc\core" (mkdir "..\dist\doc\core")
doxygen core.doxyfile

REM build .NET documentation.
msbuild cli-doc\cli.shfbproj

REM build java documentation.
cd java
call mvn javadoc:javadoc
cd ..