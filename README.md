# KString Library - Kraut Strings 🥨

A high-performance C library implementing "Kraut Strings" - a specialized string format based on the "German String" research from Umbra database system and CedarDB. KString provides a 16-byte fixed-size string representation optimized for database-like performance characteristics.

## ✨ Features

- **16-byte Fixed Size**: All string representations fit exactly in 128 bits for optimal register-based function calls
- **Dual Storage Modes**: Short strings (≤12 chars) stored inline, long strings use prefix + pointer optimization
- **High Performance**: 33% memory savings vs traditional string representations, 4 instructions vs 37 for comparisons
- **Immutable Design**: Strings cannot be modified after creation for thread safety and optimization
- **Prefix Optimization**: First 4 characters stored inline for lightning-fast comparisons
- **Complete Const Correctness**: All input parameters protected from accidental modification
- **Cross-Platform**: Supports Linux, macOS, and Windows with CMake build system

## 🚀 Quick Start

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

// Create strings
KString str1 = KStringCreate("Hello", 5);
KString str2 = KStringCreateFromCStr("World!");

// String operations
KString result = KStringConcat(str1, str2);
bool isEqual = KStringEquals(str1, str2);
size_t length = KStringSize(str1);

// Comparison
int cmp = KStringCompare(str1, str2);
bool startsWith = KStringStartsWith(result, str1);

// Memory cleanup (only for temporary strings)
KStringDestroy(result);
```

## 📊 Performance Benefits

Based on German String research from Umbra/CedarDB:

| Metric | Traditional Strings | Kraut Strings | Improvement |
|--------|-------------------|---------------|-------------|
| Memory Usage | 24 bytes | 16 bytes | **33% reduction** |
| Function Call Instructions | 37 | 4 | **90% reduction** |
| Comparison Speed | Full scan | Prefix-optimized | **Early exit** |
| Cache Performance | Poor | Excellent | **Better locality** |

## 🏗️ Architecture

### Core Design Principles

1. **16-byte Fixed Size**: Entire string representation fits in 128 bits
2. **Two Storage Modes**:
   - **Short strings (≤12 chars)**: 32-bit size + 96-bit inline content
   - **Long strings (>12 chars)**: 32-bit size + 32-bit prefix + 2-bit storage class + 62-bit pointer
3. **Immutable Strings**: No modification after creation
4. **Prefix Optimization**: First 4 characters stored inline for fast comparisons
5. **Register Passing**: 16-byte limit enables CPU register-based function calls

### Storage Classes (Long Strings)

- **`KSTRING_PERSISTENT`**: Valid forever (string literals, constants)
- **`KSTRING_TRANSIENT`**: Temporarily valid (may become invalid)
- **`KSTRING_TEMPORARY`**: Created during execution (requires cleanup)

## 📚 API Reference

### Core Operations

```c
// Secure string creation (recommended)
KString KStringCreate(const char* pStr, const size_t Size);
KString KStringCreatePersistent(const char* pStr, const size_t Size);
KString KStringCreateTransient(const char* pStr, const size_t Size);

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

## 🎯 Use Cases

Perfect for applications requiring:

- **Database Systems**: High-performance string handling like Umbra/CedarDB
- **In-Memory Analytics**: Efficient string operations with minimal memory overhead
- **High-Frequency Trading**: Ultra-fast string comparisons for market data
- **Game Engines**: Memory-efficient string handling for assets and identifiers
- **Embedded Systems**: Predictable memory usage with fixed-size strings
- **Scientific Computing**: Optimized string operations for data processing

## 🔬 Research Background

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

## 🛠️ Build Requirements

- **CMake 4.0+**: Modern CMake configuration
- **C23 Compiler**: GCC, Clang, or MSVC with C23 support
- **Ninja**: Fast parallel builds (recommended)

### Platform Support

| Platform | Shared Library | Static Library | Status |
|----------|---------------|----------------|---------|
| Linux | `libkstring.so` | `libkstring_static.a` | ✅ Tested |
| macOS | `libkstring.dylib` | `libkstring_static.a` | ✅ Tested |
| Windows | `kstring.dll` | `kstring_static.lib` | ✅ Supported |

## 📁 Project Structure

```text
KString/
├── CMakeLists.txt           # Main build configuration
├── include/
│   └── KString.h           # Public API header
├── src/
│   └── KString.c           # Implementation
├── _examples/
│   ├── CMakeLists.txt      # Example build configuration
│   └── main.c              # Demo program
├── _research/              # Research papers and documentation
├── .copilot-instructions.md # Development guidelines
└── README.md               # This file
```

## 🎨 Example Program

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

## 🤝 Contributing

We welcome contributions! Key areas for enhancement:

- Unicode/UTF-8 support for proper international character handling
- String interning for memory efficiency
- Copy-on-write semantics
- Custom allocators
- SIMD optimizations for string operations
- Integration with popular C frameworks

## 📄 License

This project implements research from the database community. Please refer to the original German String papers for academic citations.

## 🔗 Research References

- **Umbra Database System**: Original German String implementation
- **CedarDB**: Commercial database using German String optimizations
- **Christian Winter**: "A Deep Dive into German Strings"
- **Lukas Vogel**: "Why German Strings are Everywhere"

## 🙏 Acknowledgments

**Research Credit**: All performance benefits and design decisions are based on the original German String research from Umbra database system and CedarDB. "Kraut Strings" is simply our branding of this proven technology.

**Why "Kraut"?**: A playful, memorable name while honoring the German origins of this string optimization. The technical implementation remains faithful to the original research specifications.

---

**KString - Bringing database-grade string performance to C applications** 🚀
