# Pre-built releases

The latest stable releases for Windows and Linux are available under [Releases](https://github.com/Kippye/schetool/releases). Just download, extract and run.

# Building schetool

## Windows

### Prerequisites

**General**
* [CMake](https://cmake.org/) version 3.25 or later for project file generation.
* [vcpkg](https://learn.microsoft.com/en-gb/vcpkg/get_started/get-started?pivots=shell-bash) to get the some of the project's libraries - complete step 1. here.
* It would be preferred to use [Visual Studio build tools](https://visualstudio.microsoft.com/downloads/?q=build+tools#build-tools-for-visual-studio-2022) for vcpkg.

**MSVC**
* Visual Studio, at least its C++ development [build tools](https://visualstudio.microsoft.com/downloads/?q=build+tools#build-tools-for-visual-studio-2022). The project has a preset for Visual Studio 17 2022. No other versions have been tested, but might work if a preset is added.

**Makefile**
* MinGW64 (recommended to get it from [MSYS2](https://www.msys2.org/))
* A compiler (GCC (version 1.14.1 or later) and clang have been tested)
* TODO: Full package list

> [!NOTE]
> Currently, using the ld linker seems to cause linking errors. The CMakeLists automatically uses lld instead.

### Building

Tests can be enabled / disabled using the ``-DENABLE_TESTING=ON`` or ``-DENABLE_TESTING=OFF`` CMake flags.

If building with tests, the executable will be in a ``build\tests`` subdirectory.

**Using MSVC**
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
## Linux

### Prerequisites  

* [CMake](https://cmake.org/) version 3.25 or later for project file generation.
* A compiler such as GCC (version 1.14.1 or later).
* [GLFW dependency packages](https://www.glfw.org/docs/3.3/compile.html#compile_deps) - get the list for your distro and desktop environment.
* At least on Fedora, the ``mesa-libGL-devel`` package is also needed (might not be if other graphics drivers are installed). 
* [vcpkg](https://learn.microsoft.com/en-gb/vcpkg/get_started/get-started?pivots=shell-bash) to get the some of the project's libraries - complete step 1. here.
<details>
<summary>NOTE: vcpkg requires these packages, depending on your distro:</summary>

* Debian and Ubuntu derivatives:
  
  ``sudo apt-get install curl zip unzip tar``
* Recent Red Hat and Fedora derivatives:
  
  ``sudo dnf install curl zip unzip tar``
* Older Red Hat and Fedora derivatives:
  
  ``sudo yum install curl zip unzip tar``
* SUSE Linux and derivatives:
  
  ``sudo zypper install curl zip unzip tar``
* Arch Linux and derivatives:
  
  ``sudo pacman -Syu base-devel git curl zip unzip tar cmake ninja``
* Alpine:
  
  ``apk add build-base cmake ninja zip unzip curl git``
</details> 

### Building

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
