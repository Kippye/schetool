# using-winsdk-with-mingw
CMake scripts to use Windows SDK with MinGW-w64.

## Description  
You can use its scripts by copy and paste.

This script will set the following value.  

```cmake  
#Example) C:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0
include_directories(${WINDOWS_SDK_INCLUDE_DIR}) 
```

If you include this directory in CMakeLists.txt, you can include Windows SDK headers in C/C++ sources.  

```cpp
#if defined(__GNUC__)
#include <um/uiautomationclient.h> //distinguish from MinGW one.
#elif
#include <uiautomationclient.h>
#endif
```

This is a hacky approach.  

## License 
- It is provided by MIT License.

## Author 
- pit-ray
  [Email] pit-ray(at)outlook.com
