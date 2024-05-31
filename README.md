# schetool

a daily-weekly-monthly scheduling program that aims to combine the best of all others

## Why schetool?

Schetool was created to have every feature that a scheduling tool needs and is designed specifically **for** making schedules.
Schetool contains everything that's needed and nothing more. Some alternatives have file sizes and memory usages in the hundreds of megabytes. Schetool can run in the background all day and you will only notice it when you need to.

## Pre-built releases

The latest stable releases for Windows and Linux (built and tested on Fedora) are available under Releases. Just download, extract and run.

## Building schetool

Building hasn't been tested extensively, so the exact functionability with different platforms and compilers isn't known.

### Windows

**Prerequisites**

* [CMake](https://cmake.org/) version 3.25 or later for project file generation.
* TODO: Windows compilation prerequisites

* MinGW64 to build from Makefile (recommended to get it from [MSYS2](https://www.msys2.org/))

**Building**

**Tests can be enabled / disabled using the ``-DENABLE_TESTING=ON`` or ``-DENABLE_TESTING=OFF`` CMake flags.**

**If building with tests, the resulting executable for make, Visual Studio Debug or Visual Studio Release builds will be at ``build/tests/unit_tests.exe``, ``build\tests\Debug\unit_tests.exe`` or ``build\tests\Release\unit_tests.exe``, respectively**

Using MSVC
```
cmake --preset windows-msvc2022
  # Build Debug:
cmake --build build --preset msvc2022-debug
  # Build Release:
cmake --build build --preset msvc2022-release
  # Run Debug:
build\Debug\schetool
  # Run Release:
build\Release\schetool
```
Makefile

Using MSYS2 MinGW64

Open the MSYS2 MINGW64 shell launcher
```
  # Debug:
cmake --preset make
  # Release:
cmake --preset make-release
  # Build (either)
cmake --build build
  # Run:
build/schetool
```
### Linux

**Prerequisites on Fedora**  

* CMake version 3.25 or later for project file generation.
* The following packages need to be installed:
```
sudo dnf install libX11 libXcursor-devel libXrandr-devel libXinerama-devel libXi-devel mesa-libGL-devel xorg-x11-server-devel
```

**Building**

```
  # Debug:
cmake --preset make
  # Release:
cmake --preset make-release
  # Build (either)
cmake --build build
  # Run:
build/schetool
```
