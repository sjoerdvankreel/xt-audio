* The core library (C interface) is provided as both static and 
dynamic libraries for x86 and x64 windows and linux. Compile with
a recent C++ compiler (currently gcc 9 and visual studio 2019).

* C++ bindings are only distributed as statically linked libraries. 
Compile from source if you need dynamic libraries.

* Java bindings are currently on Java 8. They should be compatible
with any java version >= 8.

* .NET library (xt-cli) is currently on netstandard2.0, to allow 
both .NET framework and .NET core clients on windows and linux.

* .NET gui and sample apps are compiled for .NET core 3.1 and .NET 
framework 4.8.

* .NET gui sample application works on both windows and linux using 
mono for .NET framework. Linux .NET Core runtime (dotnet-runtime) 
does not support WinForms/WPF so no GUI apps.

* .NET Core on linux (dotnet-runtime) currently only supports x64.
.NET Core binaries can still be used on x86 linux using mono.

* The distributed binaries are linked with all supported backends. 
Backends may be disabled at compile time if compiled from source, 
for example to link only with ALSA on linux.

* Development libraries for linux are managed using package manager.
Use apt-get and the like to make sure you have installed the necessary
packages if compiling from source (taking into account selective
backend compilation).

* Development libraries for windows are built-in for DirectSound 
and WASAPI. From-source compilations with ASIO support require 
both the source code for the steinberg ASIO sdk and asmjit (asmjit
is used to allow multiple ASIO backends running simultaneously).