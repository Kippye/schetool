# Schetool

* [Releases](#releases)
* [Building schetool](#building-schetool)
  * [Linux](#linux)
    * [Requirements](#requirements)
    * [Build commands](#build-commands)  
  * [Windows](#windows)
    * [Requirements](#requirements-1)
    * [Build commands](#build-commands-1)

## Releases

The latest stable releases for Windows and Linux are available under [Releases](https://github.com/Kippye/schetool/releases). 

Windows users should use the MSI installer when possible for notifications to work.

## Building schetool

### Linux

On Linux, using the GCC compiler is recommended.

#### Requirements  

* [CMake](https://cmake.org/) version 3.25 or later for project file generation.
* A compiler such as GCC (version 1.14.1 or later).
* [GLFW dependency packages](https://www.glfw.org/docs/3.3/compile.html#compile_deps) - get the list for your distro and desktop environment.

#### Build commands

Toggle tests with the ``-DENABLE_TESTS=ON`` or ``-DENABLE_TESTS=OFF`` CMake flags.

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

### Windows

On Windows, the program can be compiled using either MSVC (recommended) or other compilers from MinGW64 (this is no longer supported or tested).

#### Requirements

* [CMake](https://cmake.org/) version 3.25 or later for project file generation.

**MSVC**
* Visual Studio, at least its C++ development [build tools](https://visualstudio.microsoft.com/downloads/?q=build+tools#build-tools-for-visual-studio-2022). The project has a preset for Visual Studio 17 2022. No other versions have been tested, but they might work.

**Makefile**
* MinGW64 (recommended to get it from [MSYS2](https://www.msys2.org/))
* A compiler \[GCC (version 1.14.1 or later) or clang (not tested)\]
* TODO: Full package list

> [!NOTE]
> The lld linker is used instead of ld.

> [!NOTE]
> Builds created using MinGW do not support sending notifications.

#### Build commands

Toggle tests with the ``-DENABLE_TESTS=ON`` or ``-DENABLE_TESTS=OFF`` CMake flags.

**MSVC**
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
**Makefile**

Open the MSYS2 MinGW64 shell launcher
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
To build using a different compiler, such as clang, pass the compiler as an argument when configuring:
```
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ --preset make
```
