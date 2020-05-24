@echo off
setLocal enableDelayedExpansion

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
set archs[0]=x86
set archs[1]=x64
set types[0]=OFF
set types[1]=ON
set libs[0]=static
set libs[1]=shared
set confs[0]=debug
set confs[1]=release
set vsarchs[0]=Win32
set vsarchs[1]=x64
set generator="Visual Studio 16 2019"

REM Build native projects.
REM For core library, all combinations of x86/64, debug/release, shared/static.
REM For all others, shared versions are not supported.
cd ..\scratch
for /L %%A in (0, 1, 1) do (
  for /L %%L in (0, 1, 1) do (
    set FS=win32-!archs[%%A]!-!libs[%%L]!
    if not exist !FS! (mkdir !FS!)
    cd !FS!
    cmake ..\..\build -G %generator% -A!vsarchs[%%A]! -DDISABLE_DSOUND=%disable_dsound% -DDISABLE_WASAPI=%disable_wasapi% -DDISABLE_ASIO=%disable_asio% -DXT_ASIOSDK_DIR=%4 -DXT_ASMJIT_DIR=%5 -DBUILD_SHARED_LIBS=!types[%%L]!
    if !errorlevel! neq 0 exit /b !errorlevel!
    for /L %%C in (0, 1, 1) do (
      msbuild xt-audio.sln /p:Configuration=!confs[%%C]!
      if !errorlevel! neq 0 exit /b !errorlevel!
      set FT=..\..\temp\core-xt-win32-!archs[%%A]!-!confs[%%C]!-!libs[%%L]!
      if not exist !FT! (mkdir !FT!)
      copy !confs[%%C]!\xt-core.dll !FT!\xt-core.dll
      if %%C == 0 (
        copy !confs[%%C]!\xt-core.pdb !FT!\xt-core.pdb
      )
      if %%C == 1 (
        set FD=..\..\dist\core-xt-win32-!archs[%%A]!-!libs[%%L]!
        if not exist !FD! (mkdir !FD!)
        copy !FT! !FD!
      )
      if %%L == 0 (
        copy !confs[%%C]!\xt-core.lib !FT!\xt-core.lib
        set FT=..\..\temp\cpp-xt-win32-!archs[%%A]!-!confs[%%C]!-!libs[%%L]!
        if not exist !FT! (mkdir !FT!)
        copy !confs[%%C]!\xt-cpp.lib !FT!\xt-cpp.lib
        copy !confs[%%C]!\xt-core.lib !FT!\xt-core.lib
        if %%C == 0 (
          copy !confs[%%C]!\xt-cpp.pdb !FT!\xt-cpp.pdb
          copy !confs[%%C]!\xt-core.pdb !FT!\xt-core.pdb
        )
        if %%C == 1 (
          set FD=..\..\dist\cpp-xt-win32-!archs[%%A]!-!libs[%%L]!
          if not exist !FD! (mkdir !FD!)
          copy !FT! !FD!
        )
        set FT=..\..\temp\cpp-driver-win32-!archs[%%A]!-!confs[%%C]!-!libs[%%L]!
        if not exist !FT! (mkdir !FT!)
        copy !confs[%%C]!\xt-cpp-driver.exe !FT!\xt-cpp-driver.exe
        if %%C == 0 (
          copy !confs[%%C]!\xt-cpp-driver.pdb !FT!\xt-cpp-driver.pdb
        )
        set FT=..\..\temp\cpp-sample-win32-!archs[%%A]!-!confs[%%C]!-!libs[%%L]!
        if not exist !FT! (mkdir !FT!)
        copy !confs[%%C]!\xt-cpp-sample.exe !FT!\xt-cpp-sample.exe
        if %%C == 0 (
          copy !confs[%%C]!\xt-cpp-sample.pdb !FT!\xt-cpp-sample.pdb
        )
        if %%C == 1 (
          set FD=..\..\dist\cpp-sample-win32-!archs[%%A]!-!libs[%%L]!
          if not exist !FD! (mkdir !FD!)
          copy !FT! !FD!
        )
      )
    )
    cd ..
  )
)
cd ..\build

REM build cli projects.
for %%C in (debug release) do (
  msbuild cli.sln /p:Configuration=%%C
  if !errorlevel! neq 0 exit /b !errorlevel!
  if not exist ..\scratch\cli\%%C\netstandard2.0\win32-x86 mkdir ..\scratch\cli\%%C\netstandard2.0\win32-x86
  copy ..\temp\core-xt-win32-x86-%%C-shared\*.* ..\scratch\cli\%%C\netstandard2.0\win32-x86
  if not exist ..\scratch\cli\%%C\netstandard2.0\win32-x64 mkdir ..\scratch\cli\%%C\netstandard2.0\win32-x64
  copy ..\temp\core-xt-win32-x64-%%C-shared\*.* ..\scratch\cli\%%C\netstandard2.0\win32-x64
  if not exist ..\scratch\cli-gui\%%C\net48\win32-x86 mkdir ..\scratch\cli-gui\%%C\net48\win32-x86
  copy ..\temp\core-xt-win32-x86-%%C-shared\*.* ..\scratch\cli-gui\%%C\net48\win32-x86
  if not exist ..\scratch\cli-gui\%%C\net48\win32-x64 mkdir ..\scratch\cli-gui\%%C\net48\win32-x64
  copy ..\temp\core-xt-win32-x64-%%C-shared\*.* ..\scratch\cli-gui\%%C\net48\win32-x64
  if not exist ..\scratch\cli-gui\%%C\netcoreapp3.1\win32-x86 mkdir ..\scratch\cli-gui\%%C\netcoreapp3.1\win32-x86
  copy ..\temp\core-xt-win32-x86-%%C-shared\*.* ..\scratch\cli-gui\%%C\netcoreapp3.1\win32-x86
  if not exist ..\scratch\cli-gui\%%C\netcoreapp3.1\win32-x64 mkdir ..\scratch\cli-gui\%%C\netcoreapp3.1\win32-x64
  copy ..\temp\core-xt-win32-x64-%%C-shared\*.* ..\scratch\cli-gui\%%C\netcoreapp3.1\win32-x64
  if not exist ..\scratch\cli-sample\%%C\net48\win32-x86 mkdir ..\scratch\cli-sample\%%C\net48\win32-x86
  copy ..\temp\core-xt-win32-x86-%%C-shared\*.* ..\scratch\cli-sample\%%C\net48\win32-x86
  if not exist ..\scratch\cli-sample\%%C\net48\win32-x64 mkdir ..\scratch\cli-sample\%%C\net48\win32-x64
  copy ..\temp\core-xt-win32-x64-%%C-shared\*.* ..\scratch\cli-sample\%%C\net48\win32-x64
  if not exist ..\scratch\cli-sample\%%C\netcoreapp3.1\win32-x86 mkdir ..\scratch\cli-sample\%%C\netcoreapp3.1\win32-x86
  copy ..\temp\core-xt-win32-x86-%%C-shared\*.* ..\scratch\cli-sample\%%C\netcoreapp3.1\win32-x86
  if not exist ..\scratch\cli-sample\%%C\netcoreapp3.1\win32-x64 mkdir ..\scratch\cli-sample\%%C\netcoreapp3.1\win32-x64
  copy ..\temp\core-xt-win32-x64-%%C-shared\*.* ..\scratch\cli-sample\%%C\netcoreapp3.1\win32-x64
  if not exist ..\temp\cli-xt-%%C\netstandard2.0 (mkdir ..\temp\cli-xt-%%C\netstandard2.0)
  xcopy /y /s ..\scratch\cli\%%C\netstandard2.0 ..\temp\cli-xt-%%C\netstandard2.0
  if not exist ..\temp\cli-gui-%%C\net48 (mkdir ..\temp\cli-gui-%%C\net48)
  xcopy /y /s ..\scratch\cli-gui\%%C\net48 ..\temp\cli-gui-%%C\net48
  if not exist ..\temp\cli-gui-%%C\netcoreapp3.1 (mkdir ..\temp\cli-gui-%%C\netcoreapp3.1)
  xcopy /y /s ..\scratch\cli-gui\%%C\netcoreapp3.1 ..\temp\cli-gui-%%C\netcoreapp3.1
  if not exist ..\temp\cli-gui32-%%C\net48 (mkdir ..\temp\cli-gui32-%%C\net48)
  xcopy /y /s ..\scratch\cli-gui\%%C\net48 ..\temp\cli-gui32-%%C\net48
  corflags ..\temp\cli-gui32-%%C\net48\xt-cli-gui.exe /32bitpref+
  copy ..\scratch\cli\%%C\netstandard2.0\xt-cli.dll.config ..\scratch\cli-gui\%%C\net48\xt-cli.dll.config
  copy ..\scratch\cli\%%C\netstandard2.0\xt-cli.dll.config ..\temp\cli-gui-%%C\net48\xt-cli.dll.config
  copy ..\scratch\cli\%%C\netstandard2.0\xt-cli.dll.config ..\temp\cli-gui32-%%C\net48\xt-cli.dll.config
  if not exist ..\temp\cli-sample-%%C\net48 (mkdir ..\temp\cli-sample-%%C\net48)
  xcopy /y /s ..\scratch\cli-sample\%%C\net48 ..\temp\cli-sample-%%C\net48
  if not exist ..\temp\cli-sample-%%C\netcoreapp3.1 (mkdir ..\temp\cli-sample-%%C\netcoreapp3.1)
  xcopy /y /s ..\scratch\cli-sample\%%C\netcoreapp3.1 ..\temp\cli-sample-%%C\netcoreapp3.1
  if not exist ..\temp\cli-sample32-%%C\net48 (mkdir ..\temp\cli-sample32-%%C\net48)
  xcopy /y /s ..\scratch\cli-sample\%%C\net48 ..\temp\cli-sample32-%%C\net48
  corflags ..\temp\cli-sample32-%%C\net48\xt-cli-sample.exe /32bitpref+
  if !errorlevel! neq 0 exit /b !errorlevel!
  copy ..\scratch\cli\%%C\netstandard2.0\xt-cli.dll.config ..\scratch\cli-sample\%%C\net48\xt-cli.dll.config
  copy ..\scratch\cli\%%C\netstandard2.0\xt-cli.dll.config ..\temp\cli-sample-%%C\net48\xt-cli.dll.config
  copy ..\scratch\cli\%%C\netstandard2.0\xt-cli.dll.config ..\temp\cli-sample32-%%C\net48\xt-cli.dll.config
)
if not exist ..\dist\cli-xt\netstandard2.0 (mkdir ..\dist\cli-xt\netstandard2.0)
if not exist ..\dist\cli-gui\net48 (mkdir ..\dist\cli-gui\net48)
if not exist ..\dist\cli-gui\netcoreapp3.1 (mkdir ..\dist\cli-gui\netcoreapp3.1)
if not exist ..\dist\cli-sample\net48 (mkdir ..\dist\cli-sample\net48)
if not exist ..\dist\cli-sample\netcoreapp3.1 (mkdir ..\dist\cli-sample\netcoreapp3.1)
xcopy /y /s ..\temp\cli-xt-release\netstandard2.0 ..\dist\cli-xt\netstandard2.0
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
if not exist ..\temp\java-xt\win32-x86 (mkdir ..\temp\java-xt\win32-x86)
if not exist ..\temp\java-xt\win32-x64 (mkdir ..\temp\java-xt\win32-x64)
if not exist ..\scratch\java\target\win32-x86 (mkdir ..\scratch\java\target\win32-x86)
if not exist ..\scratch\java\target\win32-x64 (mkdir ..\scratch\java\target\win32-x64)
copy ..\scratch\java\target\*.jar ..\temp\java-xt
copy ..\temp\core-xt-win32-x86-release-shared\xt-core.dll ..\temp\java-xt\win32-x86
copy ..\temp\core-xt-win32-x64-release-shared\xt-core.dll ..\temp\java-xt\win32-x64
copy ..\temp\core-xt-win32-x86-release-shared\xt-core.dll ..\scratch\java\target\win32-x86
copy ..\temp\core-xt-win32-x64-release-shared\xt-core.dll ..\scratch\java\target\win32-x64
if not exist ..\dist\java-xt (mkdir ..\dist\java-xt)
xcopy /y /s ..\temp\java-xt\*.* ..\dist\java-xt\*.*

cd java-driver
call mvn install
if !errorlevel! neq 0 exit /b !errorlevel!
cd ..
if not exist ..\temp\java-driver (mkdir ..\temp\java-driver)
if not exist ..\temp\java-driver\win32-x86 (mkdir ..\temp\java-driver\win32-x86)
if not exist ..\temp\java-driver\win32-x64 (mkdir ..\temp\java-driver\win32-x64)
if not exist ..\scratch\java-driver\target\win32-x86 (mkdir ..\scratch\java-driver\target\win32-x86)
if not exist ..\scratch\java-driver\target\win32-x64 (mkdir ..\scratch\java-driver\target\win32-x64)
copy ..\scratch\java-driver\target\*.jar ..\temp\java-driver
copy ..\temp\core-xt-win32-x86-release-shared\xt-core.dll ..\temp\java-driver\win32-x86
copy ..\temp\core-xt-win32-x64-release-shared\xt-core.dll ..\temp\java-driver\win32-x64
copy ..\temp\core-xt-win32-x86-release-shared\xt-core.dll ..\scratch\java-driver\target\win32-x86
copy ..\temp\core-xt-win32-x64-release-shared\xt-core.dll ..\scratch\java-driver\target\win32-x64

cd java-sample
call mvn install
if !errorlevel! neq 0 exit /b !errorlevel!
cd ..
if not exist ..\temp\java-sample (mkdir ..\temp\java-sample)
if not exist ..\temp\java-sample\win32-x86 (mkdir ..\temp\java-sample\win32-x86)
if not exist ..\temp\java-sample\win32-x64 (mkdir ..\temp\java-sample\win32-x64)
if not exist ..\scratch\java-sample\target\win32-x86 (mkdir ..\scratch\java-sample\target\win32-x86)
if not exist ..\scratch\java-sample\target\win32-x64 (mkdir ..\scratch\java-sample\target\win32-x64)
copy ..\scratch\java-sample\target\*.jar ..\temp\java-sample
copy ..\temp\core-xt-win32-x86-release-shared\xt-core.dll ..\temp\java-sample\win32-x86
copy ..\temp\core-xt-win32-x64-release-shared\xt-core.dll ..\temp\java-sample\win32-x64
copy ..\temp\core-xt-win32-x86-release-shared\xt-core.dll ..\scratch\java-sample\target\win32-x86
copy ..\temp\core-xt-win32-x64-release-shared\xt-core.dll ..\scratch\java-sample\target\win32-x64
if not exist ..\dist\java-sample (mkdir ..\dist\java-sample)
xcopy /y /s ..\temp\java-sample\*.* ..\dist\java-sample\*.*

REM build documentation.
msbuild cli-doc\cli.shfbproj
doxygen cpp.doxyfile
doxygen core.doxyfile
javadoc -Xmaxerrs 1 -Xmaxwarns 1 -sourcepath ../src/java -d ../dist/doc/java com.xtaudio.xt