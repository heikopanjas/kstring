# Using KString Library in Your Project

This document explains how to integrate the KString library into your CMake project using various methods.

## Method 1: Using FetchContent (Recommended for dependencies)

Add this to your `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
    KString
    GIT_REPOSITORY https://github.com/heikopanjas/kstring.git
    GIT_TAG        main  # or specific version tag
)

FetchContent_MakeAvailable(KString)

# Link against KString
target_link_libraries(your_target PRIVATE KString::kstring)
```

## Method 2: Using find_package (After installation)

First, install KString:

```bash
cmake -B build
cmake --build build
cmake --install build --prefix /usr/local
```

Then in your project's `CMakeLists.txt`:

```cmake
find_package(KString 1.0 REQUIRED)

# Link against KString
target_link_libraries(your_target PRIVATE KString::kstring)
```

## Method 3: Using add_subdirectory

If KString is in a subdirectory of your project:

```cmake
add_subdirectory(path/to/kstring)

# Link against KString
target_link_libraries(your_target PRIVATE KString::kstring)
```

## Method 4: Using pkg-config

After installation, you can use pkg-config:

```bash
gcc myapp.c $(pkg-config --cflags --libs kstring) -o myapp
```

Or in CMake:

```cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(KString REQUIRED kstring)

target_link_libraries(your_target PRIVATE ${KString_LIBRARIES})
target_include_directories(your_target PRIVATE ${KString_INCLUDE_DIRS})
```

## Choosing Between Shared and Static Library

By default, linking against `KString::kstring` uses the shared library.

To use the static library instead:

```cmake
target_link_libraries(your_target PRIVATE KString::kstring_static)
```

## Example Usage in Code

```c
#include <KString.h>
#include <stdio.h>

int main(void) {
    // Create a string
    const char* text = "Hello, World!";
    KString str = KStringCreate(text, 13);
    
    // Use the string
    printf("String: %s\n", KStringCStr(str));
    printf("Size: %zu bytes\n", KStringSize(str));
    
    // Clean up
    KStringDestroy(str);
    
    return 0;
}
```

## Installation Paths

By default, KString installs to:
- Headers: `${prefix}/include/KString.h`
- Libraries: `${prefix}/lib/libkstring.{so,dylib,a}`
- CMake config: `${prefix}/lib/cmake/KString/`
- pkg-config: `${prefix}/lib/pkgconfig/kstring.pc`

You can customize the installation prefix:

```bash
cmake --install build --prefix /opt/kstring
```
