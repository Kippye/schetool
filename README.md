# schetool

a daily-weekly-monthly scheduling program that aims to combine the best of all others

## Why schetool?

### It knows its purpose

Schetool was created to have every feature that a scheduling tool needs and is designed specifically **for** making schedules.
Schetool contains everything that's needed and nothing more. Some alternatives have file sizes and memory usages in the hundreds of megabytes. Schetool can run in the background all day and you will only notice it when you need to.

## Pre-built releases

The latest stable releases for Windows and Linux (built on Fedora) are available under Releases. Just download, extract and run.

## Building schetool

Building hasn't been tested extensively, so the exact functionability with different platforms and compilers isn't known.

### Windows

**Prerequisites**

* [premake5](https://premake.github.io/download) for project file generation.
* TODO: Windows compilation prerequisites

* MinGW64 to build from Makefile (recommended to get it from [MSYS2](https://www.msys2.org/))

**Building**

Visual Studio project file (BROKEN)
```
premake5 vs2022  
msbuild -m -p:Configuration=Release
```
Makefile

Using MSYS2 MinGW64

Open the MSYS2 MINGW64 shell launcher
```
premake5 gmake2
make
```
**Cleaning**
```
premake5 clean
```
### Linux

**Prerequisites on Fedora**  

* [premake5](https://premake.github.io/download) for project file generation.
* X11 packages need to be installed in order to compile:
```
sudo dnf install libX11 libXcursor-devel libXrandr-devel libXinerama-devel libXi-devel mesa-libGL-devel xorg-x11-server-devel
```

**Building**

```
premake5 gmake2  
make
```
To run:  
```
./schetool_bin
```
**Cleaning**
```
premake5 clean
```
