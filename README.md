# KString Library - Kraut Strings

A high-performance C library implementing "Kraut Strings" - a specialized string format based on the "German String" research from Umbra database system and CedarDB. KString provides a 16-byte fixed-size string representation optimized for database-like performance characteristics.

## Features

- **16-byte Fixed Size**: All string representations fit exactly in 128 bits for optimal register-based function calls
- **Dual Storage Modes**: Short strings (≤12 chars) stored inline, long strings use prefix + pointer optimization
- **Character Encoding Support**: UTF-8, UTF-16LE, UTF-16BE, and ANSI encodings with 2-bit encoding storage
- **High Performance**: 33% memory savings vs traditional string representations, 4 instructions vs 37 for comparisons
- **Immutable Design**: Strings cannot be modified after creation for thread safety and optimization
- **Prefix Optimization**: First 4 characters stored inline for lightning-fast comparisons
- **Complete Const Correctness**: All input parameters protected from accidental modification
- **Cross-Platform**: Supports Linux, macOS, and Windows with CMake build system

## Quick Start

### Building the Library

```bash
# Configure build
cmake -GNinja -B_build

# Build everything
cmake --build _build

# Run demo
./_build/_examples/kstring_demo
```

### Basic Usage

```c
#include "KString.h"

// Create strings (UTF-8 by default)
KString str1 = KStringCreate("Hello", 5);
KString str2 = KStringCreateFromCStr("World!");

// Create strings with explicit encoding
KString utf16str = KStringCreateWithEncoding("UTF-16", 6, KSTRING_ENCODING_UTF16LE);
KString ansistr = KStringCreateWithEncoding("ANSI", 4, KSTRING_ENCODING_ANSI);

// String operations
KString result = KStringConcat(str1, str2);
bool isEqual = KStringEquals(str1, str2);
size_t length = KStringSize(str1);
KStringEncoding encoding = KStringGetEncoding(str1);

// Encoding conversion
KString utf16converted = KStringConvertUtf8ToUtf16Le(str1);
KString ansiconverted = KStringConvertUtf8ToAnsi(str1);

// Comparison
int cmp = KStringCompare(str1, str2);
bool startsWith = KStringStartsWith(result, str1);

// Memory cleanup (only for temporary strings)
KStringDestroy(result);
KStringDestroy(utf16converted);
KStringDestroy(ansiconverted);
```

## Performance Benefits

Based on German String research from Umbra/CedarDB:

| Metric | Traditional Strings | Kraut Strings | Improvement |
|--------|-------------------|---------------|-------------|
| Memory Usage | 24 bytes | 16 bytes | **33% reduction** |
| Function Call Instructions | 37 | 4 | **90% reduction** |
| Comparison Speed | Full scan | Prefix-optimized | **Early exit** |
| Cache Performance | Poor | Excellent | **Better locality** |

## Architecture

### Core Design Principles

1. **16-byte Fixed Size**: Entire string representation fits in 128 bits
2. **Two Storage Modes**:
   - **Short strings (≤12 chars)**: 30-bit size + 2-bit encoding + 96-bit inline content
   - **Long strings (>12 chars)**: 30-bit size + 2-bit encoding + 32-bit prefix + 2-bit storage class + 62-bit pointer
3. **Character Encoding**: 2-bit encoding field supports UTF-8, UTF-16LE, UTF-16BE, and ANSI
4. **Immutable Strings**: No modification after creation
5. **Prefix Optimization**: First 4 characters stored inline for fast comparisons
6. **Register Passing**: 16-byte limit enables CPU register-based function calls

### Character Encodings

- **`KSTRING_ENCODING_UTF8`**: UTF-8 Unicode encoding (default)
- **`KSTRING_ENCODING_UTF16LE`**: UTF-16 Little Endian
- **`KSTRING_ENCODING_UTF16BE`**: UTF-16 Big Endian
- **`KSTRING_ENCODING_ANSI`**: ANSI/Windows-1252 (legacy Windows compatibility)

### Storage Classes (Long Strings)

- **`KSTRING_PERSISTENT`**: Valid forever (string literals, constants)
- **`KSTRING_TRANSIENT`**: Temporarily valid (may become invalid)
- **`KSTRING_TEMPORARY`**: Created during execution (requires cleanup)

## API Reference

### Core Operations

```c
// Secure string creation (recommended)
KString KStringCreate(const char* pStr, const size_t Size);
KString KStringCreatePersistent(const char* pStr, const size_t Size);
KString KStringCreateTransient(const char* pStr, const size_t Size);

// Encoding-aware string creation
KString KStringCreateWithEncoding(const char* pStr, const size_t Size, const KStringEncoding Encoding);
KString KStringCreatePersistentWithEncoding(const char* pStr, const size_t Size, const KStringEncoding Encoding);
KString KStringCreateTransientWithEncoding(const char* pStr, const size_t Size, const KStringEncoding Encoding);

// Convenience functions (less secure)
KString KStringCreateFromCStr(const char* pStr);
KString KStringCreatePersistentFromCStr(const char* pStr);

// Memory management
void KStringDestroy(const KString Str);
```

### Access Operations

```c
// Get string properties
const char* KStringCStr(const KString Str);
size_t KStringSize(const KString Str);
KStringEncoding KStringGetEncoding(const KString Str);
bool KStringIsShort(const KString Str);
bool KStringIsValid(const KString Str);
```

### Comparison Operations

```c
// Standard comparisons
int KStringCompare(const KString StrA, const KString StrB);
bool KStringEquals(const KString StrA, const KString StrB);
bool KStringStartsWith(const KString Str, const KString Prefix);

// Case-insensitive comparisons
int KStringCompareIgnoreCase(const KString StrA, const KString StrB);
bool KStringEqualsIgnoreCase(const KString StrA, const KString StrB);
bool KStringStartsWithIgnoreCase(const KString Str, const KString Prefix);
```

### String Operations

```c
// Create new strings
KString KStringConcat(const KString StrA, const KString StrB);
KString KStringSubstring(const KString Str, const size_t Offset, const size_t Size);

// Error handling
KString KStringInvalid(void);
```

### Encoding Conversion Operations

```c
// Convert string to different encoding
KString KStringConvertToEncoding(const KString Str, const KStringEncoding TargetEncoding);

// UTF-8 <-> UTF-16LE conversion
KString KStringConvertUtf8ToUtf16Le(const KString Str);
KString KStringConvertUtf16LeToUtf8(const KString Str);

// UTF-8 <-> UTF-16BE conversion
KString KStringConvertUtf8ToUtf16Be(const KString Str);
KString KStringConvertUtf16BeToUtf8(const KString Str);

// UTF-16LE <-> UTF-16BE conversion
KString KStringConvertUtf16LeToUtf16Be(const KString Str);
KString KStringConvertUtf16BeToUtf16Le(const KString Str);

// UTF-8 <-> ANSI conversion (Windows-1252)
KString KStringConvertUtf8ToAnsi(const KString Str);
KString KStringConvertAnsiToUtf8(const KString Str);
```

## Use Cases

Perfect for applications requiring:

- **Database Systems**: High-performance string handling like Umbra/CedarDB
- **In-Memory Analytics**: Efficient string operations with minimal memory overhead
- **High-Frequency Trading**: Ultra-fast string comparisons for market data
- **Game Engines**: Memory-efficient string handling for assets and identifiers
- **Embedded Systems**: Predictable memory usage with fixed-size strings
- **Scientific Computing**: Optimized string operations for data processing

## Research Background

KString implements the "German String" format from database research:

### Key Research Insights

- **String Prevalence**: Strings make up ~50% of data processed in real-world systems
- **Short String Dominance**: Most strings are short (ISO codes, enums, IDs ≤12 chars)
- **Read-Heavy Workloads**: Strings are read much more often than modified
- **Prefix-Based Operations**: Many operations only need string prefixes (comparisons, sorting)

### Performance Characteristics

- **Best Case**: Short strings (≤12 chars) - zero pointer dereferences
- **Optimized Case**: Long string prefix comparisons - single register comparison
- **Trade-off**: String modification is expensive (requires reallocation)
- **Limitation**: Maximum string size 4GB (32-bit size field)

## Using KString in Your Project

KString can be integrated into your CMake project using multiple methods:

### Method 1: FetchContent (Recommended)

The easiest way to use KString as a dependency:

```cmake
include(FetchContent)

FetchContent_Declare(
    KString
    GIT_REPOSITORY https://github.com/heikopanjas/kstring.git
    GIT_TAG        v1.0.0  # or main for latest
)

FetchContent_MakeAvailable(KString)

# Link against KString
target_link_libraries(your_target PRIVATE KString::kstring)
```

**Example usage:**

```c
#include <KString.h>
#include <stdio.h>

int main(void) {
    const char* text = "FetchContent works!";
    KString str = KStringCreate(text, 19);
    printf("%s\n", KStringCStr(str));
    KStringDestroy(str);
    return 0;
}
```

### Method 2: find_package() (After Installation)

First, install KString system-wide:

```bash
cmake -B build
cmake --build build
sudo cmake --install build
```

Then in your project:

```cmake
find_package(KString 1.0 REQUIRED)

target_link_libraries(your_target PRIVATE KString::kstring)
```

### Method 3: add_subdirectory()

If KString is a subdirectory in your project:

```cmake
add_subdirectory(external/kstring)

target_link_libraries(your_target PRIVATE KString::kstring)
```

### Method 4: pkg-config

For non-CMake projects, use pkg-config after installation:

```bash
# Compile your application
gcc myapp.c $(pkg-config --cflags --libs kstring) -o myapp
```

Or in a Makefile:

```makefile
CFLAGS += $(shell pkg-config --cflags kstring)
LDFLAGS += $(shell pkg-config --libs kstring)
```

### Choosing Between Shared and Static Library

By default, `KString::kstring` links to the shared library. For the static library:

```cmake
target_link_libraries(your_target PRIVATE KString::kstring_static)
```

### Installation Paths

Default installation locations:

- **Headers**: `${prefix}/include/KString.h`
- **Libraries**: `${prefix}/lib/libkstring.{so,dylib,dll}` (shared), `${prefix}/lib/libkstring.a` or `${prefix}/lib/kstring.lib` (static)
- **CMake config**: `${prefix}/lib/cmake/KString/`
- **pkg-config**: `${prefix}/lib/pkgconfig/kstring.pc`

Customize with:

```bash
cmake --install build --prefix /opt/kstring
```

## Build Requirements

- **CMake 3.30+**: Modern CMake configuration
- **C17 Compiler**: GCC, Clang, or MSVC with C17 support
- **Ninja**: Fast parallel builds (recommended)

### Platform Support

| Platform | Shared Library | Static Library | Status |
|----------|---------------|----------------|---------|
| Linux | `libkstring.so` | `libkstring.a` | ✅ Tested |
| macOS | `libkstring.dylib` | `libkstring.a` | ✅ Tested |
| Windows | `kstring.dll` | `kstring.lib` | ✅ Supported |

## Project Structure

```text
KString/
├── CMakeLists.txt           # Main build configuration
├── build.sh                 # Build script (Linux/macOS)
├── build.ps1                # Build script (Windows PowerShell)
├── cmake/                   # CMake configuration files
│   ├── kstring.pc.in       # pkg-config template
│   └── KStringConfig.cmake.in # CMake config template
├── include/
│   └── KString.h           # Public API header
├── src/
│   └── KString.c           # Implementation
├── _examples/
│   ├── CMakeLists.txt      # Example build configuration
│   └── main.c              # Demo program
├── _research/              # Research papers and documentation
├── .github/
│   └── copilot-instructions.md # Copilot configuration
├── AGENTS.md               # AI agent development guidelines
└── README.md               # This file
```

## Example Program

The included demo showcases all major features:

```bash
./_build/_examples/kstring_demo
```

**Output:**

```text
KString Library Demo - Kraut Strings
====================================

Testing our German String (Kraut String) implementation...

1. Basic String Creation and Properties
---------------------------------------
Short string: "Hello!" (length: 6, is_short: true)
Long string: "This is a longer string that exceeds 12 characters" (length: 50, is_short: false)

[... comprehensive demo output ...]

✅ Kraut String implementation working correctly!
   Based on German String research from Umbra/CedarDB
   16-byte fixed size, optimized for performance
```

## License

This project implements research from the database community. Please refer to the original German String papers for academic citations.

## Research References

- **Thomas Neumann, Michael Freitag**: ["Umbra: A Disk-Based System with In-Memory Performance"](https://db.in.tum.de/~freitag/papers/p29-neumann-cidr20.pdf) - Technische Universität München - Original German String implementation in Umbra database system
- **Christian Winter**: ["A Deep Dive into German Strings"](https://cedardb.com/blog/strings_deep_dive/) - CedarDB blog post with detailed implementation insights
- **Lukas Vogel**: ["Why German Strings are Everywhere"](https://cedardb.com/blog/german_strings/#short-string-representation) - CedarDB blog post explaining the widespread adoption and advantages of German Strings
- **CedarDB**: Commercial database system using German String optimizations in production

## Acknowledgments

**Research Credit**: All performance benefits and design decisions are based on the original German String research from Umbra database system and CedarDB. "Kraut Strings" is simply our branding of this proven technology.

**Why "Kraut"?**: A playful, memorable name while honoring the German origins of this string optimization. The technical implementation remains faithful to the original research specifications.

---

**KString - Bringing database-grade string performance to C applications**
