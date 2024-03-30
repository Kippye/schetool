# schetool

a daily-weekly-monthly scheduling program that aims to combine the best of all others

## Why schetool?

### It knows its purpose

Schetool was created to have every feature that a scheduling tool needs and is designed specifically **for** making schedules. The purpose of schetool isn't just to **allow for** making schedules, it is **made to** make schedules.

### It knows nothing more
Schetool contains everything that's needed and nothing more. Some alternatives have file sizes and memory usages in the hundreds of megabytes. Schetool can run in the background all day and you will only notice it when you are supposed to.

## How do I use it?

### Pre-built releases

The latest stable release is available under Releases. Just download it, extract and run.

### Building schetool

Building hasn't been tested extensively, so the exact functionability with different platforms and compilers isn't known.

#### Windows

**Prerequisites**

* Schetool uses [premake5](https://premake.github.io/download) for project file generation.
* TODO: Windows compilation prerequisites

**Building**

```
premake5 vs2022  
msbuild -m -p:Configuration=Release
```
To clean, call:  
```
rmdir /S /Q obj rmdir /S /Q schetool rmdir /S /Q Debug rmdir /S /Q Release rmdir /S /Q bin  
del /Q schetool.pdb  
del /Q %~dp0\include\blf\blf.vcxproj  
del /Q %~dp0\include\zlib\zlib.vcxproj
```
#### Linux  

```
premake5 gmake2  
make
```
To run:  
```
./schetool_bin
```
