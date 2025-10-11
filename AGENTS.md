# KString Library - Copilot Instructions

**Last updated:** October 11, 2025

## Project Overview

KString is a high-performance C library implementing "Kraut Strings" - a specialized string type based on the "German String" format from database research. The library implements the exact specifications from Umbra/CedarDB research but uses the more playful name "Kraut Strings" (hence "KString") as our own branding of this proven string optimization technique.

## Naming Convention

**"Kraut Strings"** is our implementation name for what the research literature calls "German Strings":
- **Research Name**: "German Strings" (from Umbra database system and CedarDB)
- **Our Implementation**: "Kraut Strings" (KString library)
- **Why "Kraut"**: A playful, memorable name while honoring the German origins of this string optimization
- **Technical Equivalence**: KString implements the exact same 16-byte structure and optimizations as German Strings

## Project Goals

- **Pure C Implementation**: This project is exclusively written in C (C17 standard). No C++ code should be added.
- **Kraut String Implementation**: Implement the "German String" format from Umbra/CedarDB research as "Kraut Strings"
- **128-bit Fixed Size**: All string representations fit in exactly 16 bytes for register-based function calls
- **High Performance**: Focus on speed and memory efficiency similar to database string implementations
- **Immutable Design**: Strings are immutable after creation for thread safety and optimization
- **Simple API**: Provide a clean, easy-to-use interface for string operations

## Kraut String Architecture (based on German String research)

Based on research from Umbra database system and CedarDB, the KString implements:

### Core Design Principles
1. **16-byte Fixed Size**: Entire string representation fits in 128 bits
2. **Two Representations**:
   - **Short strings (≤12 chars)**: 30-bit length + 2-bit encoding + 96-bit inline content
   - **Long strings (>12 chars)**: 30-bit length + 2-bit encoding + 32-bit prefix + 2-bit storage class + 62-bit pointer
3. **Character Encoding Support**: 2-bit encoding field supports UTF-8, UTF-16LE, UTF-16BE, and ANSI
4. **Immutable Strings**: No modification after creation
5. **Prefix Optimization**: First 4 characters stored inline for fast comparisons
6. **Register Passing**: 16-byte limit enables CPU register-based function calls

### Character Encodings
- **UTF-8**: Default Unicode encoding (00)
- **UTF-16LE**: UTF-16 Little Endian for Windows compatibility (01)
- **UTF-16BE**: UTF-16 Big Endian for network protocols (10)
- **ANSI**: Windows-1252 for legacy Windows systems (11)

### Performance Optimizations
- **Memory Efficiency**: 33% memory savings vs std::string (16 vs 24 bytes)
- **Fast Prefix Comparisons**: Compare first 4 chars without pointer dereference
- **Short String Optimization**: Strings ≤12 chars stored entirely inline
- **Early Comparison Exit**: Most string comparisons fail on prefix/length check
- **Zero Copy Access**: Direct access to short string content
- **Encoding Awareness**: Zero-overhead encoding detection via inline storage

### Storage Classes (Long Strings)
- **Persistent**: Valid forever (string literals, constants)
- **Transient**: Temporarily valid (may become invalid)
- **Temporary**: Created during execution (requires cleanup)

## Architecture

### Directory Structure
```
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
└── _build/                 # Build artifacts (generated)
```

### Core Components

1. **KString Structure**: Main string data structure
   - Opaque handle (implementation hidden from users)
   - Memory-efficient storage
   - Optimized for German text patterns

2. **API Functions** (implemented):
   - `KStringCreate(pStr, Size)` - Secure string creation with explicit byte count (UTF-8 default)
   - `KStringCreatePersistent(pStr, Size)` - Persistent string with explicit byte count (UTF-8 default)
   - `KStringCreateTransient(pStr, Size)` - Transient string with explicit byte count (UTF-8 default)
   - `KStringCreateWithEncoding(pStr, Size, Encoding)` - String creation with explicit encoding
   - `KStringCreatePersistentWithEncoding(pStr, Size, Encoding)` - Persistent string with explicit encoding
   - `KStringCreateTransientWithEncoding(pStr, Size, Encoding)` - Transient string with explicit encoding
   - `KStringCreateFromCStr(pStr)` - Convenience function for null-terminated strings (less secure)
   - `KStringCreatePersistentFromCStr(pStr)` - Convenience function for persistent null-terminated strings (less secure)
   - `KStringDestroy()` - Memory cleanup
   - `KStringSize()` - Get string size in bytes
   - `KStringGetEncoding()` - Get character encoding
   - `KStringCStr()` - C-string conversion
   - `KStringConcat()` - String concatenation
   - `KStringCompare()` - String comparison
   - `KStringEquals()` - String equality check
   - `KStringStartsWith()` - Prefix testing
   - `KStringSubstring(str, offset, Size)` - Extract substring with explicit byte count and offset
   - `KStringCompareIgnoreCase()` - Case-insensitive comparison
   - `KStringEqualsIgnoreCase()` - Case-insensitive equality
   - `KStringStartsWithIgnoreCase()` - Case-insensitive prefix testing
   - `KStringConvertToEncoding(str, targetEncoding)` - Generic encoding conversion
   - `KStringConvertUtf8ToUtf16Le()` - UTF-8 to UTF-16LE conversion
   - `KStringConvertUtf16LeToUtf8()` - UTF-16LE to UTF-8 conversion
   - `KStringConvertUtf8ToUtf16Be()` - UTF-8 to UTF-16BE conversion
   - `KStringConvertUtf16BeToUtf8()` - UTF-16BE to UTF-8 conversion
   - `KStringConvertUtf16LeToUtf16Be()` - UTF-16 byte order conversion
   - `KStringConvertUtf16BeToUtf16Le()` - UTF-16 byte order conversion
   - `KStringConvertUtf8ToAnsi()` - UTF-8 to ANSI/Windows-1252 conversion
   - `KStringConvertAnsiToUtf8()` - ANSI/Windows-1252 to UTF-8 conversion

## Technical Requirements

### Build System
- **CMake 3.30+**: Modern CMake configuration with cross-platform support
- **Multi-Platform**: Supports Linux, macOS, and Windows
- **Ninja Generator**: Fast parallel builds (`cmake -GNinja -B_build`)
- **Library Types**:
  - **Shared Library**:
    - Linux: `libkstring.so`
    - macOS: `libkstring.dylib`
    - Windows: `kstring.dll`
  - **Static Library**:
    - Linux/macOS: `libkstring.a`
    - Windows: `kstring.lib`

### Coding Standards
- **C17 Standard**: Use C17 standard for broad compiler compatibility including MSVC
- **Const Correctness**: All KString input parameters are const to prevent accidental modification and improve maintainability
- **Constant-Left Comparisons**: Always place constants on the left side of comparisons (e.g., `NULL == ptr`, `0 == value`)
- **Parameter Naming Conventions**:
  - **`Size`**: Count of bytes (for byte array parameters)
  - **`cchSize`**: Count of characters (for character array parameters)
  - **Size vs Length**: Use "Size" for parameters, "Size" for internal variables and return values
- **Secure API Design**: All functions taking char* pointers require explicit length parameters to avoid unsafe CRT functions like `strlen()`
- **Strict Warnings**: Build with `-Wall -Wextra -Wpedantic` (GCC/Clang) or `/W4` (MSVC)
- **No Global State**: Thread-safe design for all platforms
- **Consistent Naming**: Use `KString` prefix for all public functions (PascalCase)
- **Memory Safety**: Proper allocation/deallocation patterns
- **Platform Portability**: Write code that compiles and runs on Linux, macOS, and Windows
- **Git Commands**: Always use traditional git commands via `run_in_terminal` tool (never use GitKraken MCP functions - they are unreliable)

## Kraut String Research Context

The library implements "Kraut Strings" based on the "German String" format from database research (Umbra/CedarDB):

### Key Research Insights
- **String Prevalence**: Strings make up ~50% of data processed in real-world systems
- **Short String Dominance**: Most strings are short (ISO codes, enums, IDs ≤12 chars)
- **Read-Heavy Workloads**: Strings are read much more often than modified
- **Prefix-Based Operations**: Many operations only need string prefixes (comparisons, sorting)

### Kraut String Benefits (from German String research)
- **Space Savings**: 33% reduction in memory usage vs traditional string representations
- **Function Call Efficiency**: 4 instructions vs 37 instructions for string comparisons
- **Cache Performance**: Better cache locality due to compact representation
- **Comparison Speed**: Most comparisons resolved without full string access
- **Thread Safety**: Immutable design eliminates need for locking

### Performance Characteristics
- **Best Case**: Short strings (≤12 chars) - zero pointer dereferences
- **Optimized Case**: Long string prefix comparisons - single register comparison
- **Trade-off**: String modification is expensive (requires reallocation)
- **Limitation**: Maximum string length 4GB (32-bit length field)

## Platform Support

### Supported Platforms
- **Linux**: GCC/Clang with glibc
- **macOS**: Clang with system libraries
- **Windows**: MSVC, MinGW, or Clang

### Platform-Specific Considerations
- **File Extensions**: CMake automatically handles platform-specific library extensions
- **Compiler Differences**: Use feature detection for platform-specific optimizations
- **Character Encoding**: Ensure consistent UTF-8 handling across platforms
- **Memory Alignment**: Consider platform-specific alignment requirements
- **Export Symbols**: Use proper symbol visibility on Windows (DLL export/import)

### Cross-Platform Guidelines
- Use standard C17 features only
- Avoid platform-specific system calls
- Use CMake for build configuration portability
- Test on all target platforms regularly
- Handle path separators and line endings appropriately

## German String Research Context

The library is based on research about German string handling in database systems. Key insights:
- German strings are often longer due to compound words
- Special characters require careful encoding handling
- Database systems use specialized string representations for German text
- Memory layout optimizations can significantly improve performance

## Implementation Guidelines

### KString Structure (16 bytes total)
```c
typedef struct KString {
    uint32_t size;        // 30-bit size (max ~1GB) + 2-bit encoding
    union {
        // Short string (≤12 chars): inline content
        char content[12];

        // Long string (>12 chars): prefix + pointer with storage class
        struct {
            char prefix[4];     // First 4 characters
            uint64_t ptr_and_class; // 62-bit pointer + 2-bit storage class
        } long_str;
    };
} KString;
```

**Note**: The size field uses 30 bits for string length (max ~1GB) and 2 bits for character encoding. All public API functions use `size_t` for size parameters and return values to follow standard C conventions.

### Character Encodings
```c
typedef enum {
    KSTRING_ENCODING_UTF8     = 0,  // Default UTF-8 encoding
    KSTRING_ENCODING_UTF16LE  = 1,  // UTF-16 Little Endian
    KSTRING_ENCODING_UTF16BE  = 2,  // UTF-16 Big Endian
    KSTRING_ENCODING_ANSI     = 3   // ANSI/Windows-1252 (legacy Windows)
} KStringEncoding;
```

### Storage Classes (for long strings)
```c
typedef enum {
    KSTRING_PERSISTENT = 0,  // Valid forever (literals, constants)
    KSTRING_TRANSIENT = 1,   // Temporarily valid
    KSTRING_TEMPORARY = 2    // Needs cleanup
} KStringStorageClass;
```

### Function Signatures
```c
// Core operations (secure API - requires explicit size)
KString KStringCreate(const char* pStr, const size_t Size);           // Size = count of bytes (UTF-8 default)
KString KStringCreatePersistent(const char* pStr, const size_t Size); // Size = count of bytes (UTF-8 default)
KString KStringCreateTransient(const char* pStr, const size_t Size);  // Size = count of bytes (UTF-8 default)

// Encoding-aware operations
KString KStringCreateWithEncoding(const char* pStr, const size_t Size, const KStringEncoding Encoding);
KString KStringCreatePersistentWithEncoding(const char* pStr, const size_t Size, const KStringEncoding Encoding);
KString KStringCreateTransientWithEncoding(const char* pStr, const size_t Size, const KStringEncoding Encoding);

// Convenience functions (less secure - use with caution)
KString KStringCreateFromCStr(const char* pStr);          // Uses strlen() internally
KString KStringCreatePersistentFromCStr(const char* pStr); // Uses strlen() internally

void KStringDestroy(const KString kstr);  // Only needed for temporary strings

// Access operations
const char* KStringCStr(const KString kstr);         // Get C-string (may allocate for short strings)
size_t KStringSize(const KString kstr);              // Get size in bytes (always fast)
KStringEncoding KStringGetEncoding(const KString kstr); // Get character encoding
bool KStringIsShort(const KString kstr);             // Check if string is stored inline

// Comparison operations (optimized with prefix)
int KStringCompare(const KString a, const KString b);
bool KStringEquals(const KString a, const KString b);
bool KStringStartsWith(const KString str, const KString prefix);

// String operations (create new strings)
KString KStringConcat(const KString a, const KString b);
KString KStringSubstring(const KString str, const size_t start, const size_t length);
```

### Key Implementation Rules
1. **Pass by Value**: KString is exactly 16 bytes - pass by value for register efficiency
2. **Size Extraction**: Always use GetSizeFromField() to extract 30-bit size from size field
3. **Encoding Preservation**: String operations inherit encoding from source strings
4. **Prefix Optimization**: Use 4-byte prefix for fast long string comparisons
5. **Immutable Design**: Never modify KString content after creation
6. **Storage Class Awareness**: Handle different lifetimes appropriately
7. **Bit Field Safety**: Use helper functions for size/encoding field manipulation

### Critical Implementation Details

#### Size Field Bit Layout (32 bits total)
```c
#define KSTRING_SIZE_MASK         0x3FFFFFFF  // 30-bit size mask (max ~1GB)
#define KSTRING_ENCODING_MASK     0xC0000000  // 2-bit encoding mask
#define KSTRING_ENCODING_SHIFT    30

// Helper functions for size field manipulation
static inline size_t GetSizeFromField(uint32_t SizeField);
static inline KStringEncoding GetEncodingFromField(uint32_t SizeField);
static inline uint32_t CreateSizeField(size_t Size, KStringEncoding Encoding);
```

#### Short String Detection
```c
#define KSTRING_MAX_SHORT_LENGTH 12
static inline bool is_short_string(KString kstr) {
    return GetSizeFromField(kstr.size) <= KSTRING_MAX_SHORT_LENGTH;
}
```

#### Pointer Tagging (Long Strings)
- Use upper 2 bits of 64-bit pointer for storage class
- Mask out tag bits before dereferencing: `ptr & 0x3FFFFFFFFFFFFFFF`
- Storage class: `(ptr >> 62) & 0x3`

#### Fast Comparison Strategy
1. Extract actual size from both strings using GetSizeFromField()
2. Compare sizes (30-bit integer comparison)
3. If different sizes → not equal
4. Compare prefix (32-bit integer comparison of first 4 chars)
5. If different prefixes → not equal
6. Only then compare full content if needed

#### Memory Layout Considerations
- Ensure 16-byte alignment for optimal register usage
- Use `static_assert(sizeof(KString) == 16)` to verify size
- Consider endianness for cross-platform compatibility

#### API Design Standards
- **Always use `size_t`** for size and size parameters in public functions
- **Always use `size_t`** for size and size return values
- Internal structure uses 30-bit size field (max ~1GB strings) + 2-bit encoding
- Use helper functions to extract size/encoding from internal 32-bit field
- Check for size overflow when converting from `size_t` to 30-bit internal size
- Encoding parameters use `KStringEncoding` enum type
- Default encoding is UTF-8 for all creation functions without explicit encoding

### Error Handling
- Return invalid KString (length = UINT32_MAX) for allocation failures
- Use defensive programming (check parameters)
- Document error conditions clearly
- No exceptions (pure C)

### Memory Management
- Caller responsible for calling `KStringDestroy()`
- Functions that return `KString*` transfer ownership
- Functions that take `const KString*` do not modify input
- Use RAII-style patterns where possible
- All dynamic memory allocation uses `calloc()` for zero-initialization (0x00 fill)

## Testing Strategy

### Example Program
- `_examples/main.c` demonstrates basic usage
- Should be updated as new features are implemented
- Serves as both demo and basic integration test

### Future Testing
- Unit tests for each function
- Performance benchmarks
- German text-specific test cases
- Memory leak detection

## Build Commands

### Recommended: Build Scripts (Easy)
The project includes comprehensive build scripts for all platforms:

**Linux/macOS:**
```bash
./build.sh              # Standard release build
./build.sh --help       # Show all options
./build.sh --clean      # Clean and build
./build.sh -c -e        # Clean, build, and run examples
./build.sh -d -v        # Debug build with verbose output
```

**Windows (PowerShell):**
```powershell
.\build.ps1              # Standard release build
.\build.ps1 -Help        # Show all options
.\build.ps1 -Clean       # Clean and build
.\build.ps1 -Clean -Examples  # Clean, build, and run examples
.\build.ps1 -Debug -Verbose   # Debug build with verbose output
```

**Build Script Options:**
- `-c, --clean` / `-Clean`: Clean build directory before building
- `-d, --debug` / `-Debug`: Build in Debug mode (default: Release)
- `-v, --verbose` / `-Verbose`: Enable verbose build output
- `-e, --examples` / `-Examples`: Run examples after building
- `-t, --test` / `-Test`: Run tests after building
- `-g, --generator` / `-Generator`: Specify CMake generator (default: Ninja)
- `-h, --help` / `-Help`: Show help message

### Manual CMake Commands

**All Platforms:**
```bash
# Configure build
cmake -GNinja -B_build

# Build everything
cmake --build _build

# Run demo
./_build/_examples/kstring_demo       # Linux/macOS
_build\_examples\kstring_demo.exe     # Windows
```

**Platform-Specific Clean Rebuild:**
```bash
# Linux/macOS
rm -rf _build && cmake -GNinja -B_build && cmake --build _build

# Windows (PowerShell)
Remove-Item -Recurse -Force _build; cmake -GNinja -B_build; cmake --build _build

# Windows (Command Prompt)
rmdir /s /q _build && cmake -GNinja -B_build && cmake --build _build
```

### Generated Libraries
- **Linux**: `_build/libkstring.so`, `_build/libkstring.a`
- **macOS**: `_build/libkstring.dylib`, `_build/libkstring.a`
- **Windows**: `_build/kstring.dll`, `_build/kstring.lib`

## Development Workflow

1. **Implement API**: Start with basic functions in `KString.h` and `KString.c`
2. **Test Incrementally**: Update example program to test new features
3. **Optimize**: Apply German string optimizations based on research
4. **Document**: Keep this file updated with implementation details

### **Commit Policy - CRITICAL**
**NEVER commit automatically** - Always wait for explicit "commit the changes" instruction.

**Why NEVER Commit Automatically:**
1. **Review Opportunity**: User needs chance to review changes before they're committed
2. **Repository Stability**: Auto-committing can leave the repository in an unstable state
3. **Documentation Sync**: Updating copilot instructions after committing code changes creates inconsistency
4. **User Control**: User maintains full control over what gets committed and when

**Correct Workflow:**
1. Make code changes when requested
2. Verify changes work (build, test)
3. **STOP** - Wait for explicit "commit the changes" instruction
4. Only then stage and commit with proper conventional commit message
5. Update copilot instructions if needed as separate step

This ensures user can review changes, make additional modifications, test thoroughly, decide timing, and maintain repository stability.

### **Commit Message Guidelines - CRITICAL**

Follow these rules to prevent VSCode terminal crashes and ensure clean git history:

**Message Format (Conventional Commits):**

```text
<type>(<scope>): <subject>

<body>

<footer>
```

**Character Limits:**

- **Subject line**: Maximum 50 characters (strict limit)
- **Body lines**: Wrap at 72 characters per line
- **Total message**: Keep under 500 characters total
- **Blank line**: Always add blank line between subject and body

**Subject Line Rules:**

- Use conventional commit types: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`, `build`, `ci`, `perf`
- Scope is optional but recommended: `feat(api):`, `fix(build):`, `docs(readme):`
- Use imperative mood: "add feature" not "added feature"
- No period at end of subject line
- Keep concise and descriptive

**Body Rules (if needed):**

- Add blank line after subject before body
- Wrap each line at 72 characters maximum
- Explain what and why, not how
- Use bullet points (`-`) for multiple items with lowercase text after bullet
- Keep it concise

**Special Character Safety:**

- Avoid nested quotes or complex quoting
- Avoid special shell characters: `$`, `` ` ``, `!`, `\`, `|`, `&`, `;`
- Use simple punctuation only
- No emoji or unicode characters

**Best Practices:**

- **Break up large commits**: Split into smaller, focused commits with shorter messages
- **One concern per commit**: Each commit should address one specific change
- **Test before committing**: Ensure code builds and works
- **Reference issues**: Use `#123` format in footer if applicable

**Examples:**

Good:

```text
feat(api): add KStringTrim function

- add trimming function to remove whitespace from
  both ends of string
- supports all encodings
```

Good (short):

```text
fix(build): correct static library output name
```

Bad (too long):

```text
feat(api): add a new comprehensive string trimming function that handles all edge cases including UTF-8, UTF-16LE, UTF-16BE, and ANSI encodings with proper boundary checking and memory management
```

Bad (special characters):

```text
fix: update `KString` with "nested 'quotes'" & $special chars!
```

## Code Style

- **Formatting**: Automatically handled by `.clang-format` configuration file
- **Comments**: Use `//` for all comments, including multiline comments
- **Naming**:
  - **Functions**: PascalCase with `KString` prefix (e.g., `KStringCreate`)
  - **Types**: PascalCase (e.g., `KString`)
  - **Local variables**: PascalCase (e.g., `MyVariable`)
  - **Function parameters**: PascalCase (e.g., `InputString`)
  - **Pointer variables**: PascalCase with `p` prefix (e.g., `pFormatInstructions`, `pStringData`)

## Important Notes

- **No C++**: This is a pure C project - do not add C++ code
- **Research-Driven**: Implementation should be based on the research in `_research/`
- **Performance Focus**: Every design decision should consider performance impact
- **Kraut String Fidelity**: Maintain exact compatibility with German String specifications
- **Naming Convention**: We call them "Kraut Strings" but implement German String research exactly

## About the Name "Kraut Strings"

**Why "Kraut" instead of "German"?**
- **Memorable Branding**: "Kraut Strings" is more distinctive and memorable than "German Strings"
- **Playful Homage**: Honors the German origins of this optimization in a lighthearted way
- **Technical Accuracy**: KString implements the exact same specifications as German Strings from research
- **Differentiation**: Distinguishes our C implementation from other German String implementations

**Research Credit**: All performance benefits and design decisions are based on the original German String research from Umbra database system and CedarDB. "Kraut Strings" is simply our branding of this proven technology.

## Future Enhancements

- Unicode/UTF-8 support for proper German character handling
- String interning for memory efficiency
- Copy-on-write semantics
- Custom allocators
- SIMD optimizations for string operations
- Integration with popular C frameworks

## Recent Updates & Decisions

### October 11, 2025

- **C standard downgraded to C17**: Changed from C23 to C17 for broader compiler compatibility, specifically to support MSVC on Windows. Removed C23-specific features like digit separators in numeric literals (e.g., `0x3FFF'FFFF` became `0x3FFFFFFF`). This ensures the library can be built with current versions of MSVC which don't yet support C23, while maintaining all functionality and performance characteristics.
- **GitHub Actions CI workflow added**: Created comprehensive CI workflow with matrix builds for Linux, macOS, and Windows using platform-specific generators (Unix Makefiles for Linux/macOS, Visual Studio 2022 for Windows). Workflow triggers on develop and feature branches, creates platform-specific tar.gz artifacts with build numbers and dates, and uploads them for 7-day retention. Initial Windows support pending MSVC C23 compatibility.
- **Commit message guidelines added**: Added comprehensive commit message guidelines to prevent VSCode terminal crashes. Guidelines include strict character limits (50 chars for subject, 72 chars per body line, 500 total), proper conventional commits format, special character safety rules, and examples of good vs bad commit messages. This addresses past issues where overly long or improperly formatted commit messages would crash the VSCode terminal during git operations.
- **Comprehensive build scripts added**: Created `build.sh` (Linux/macOS) and `build.ps1` (Windows PowerShell) scripts to simplify the build process. These scripts provide dependency checking, clean builds, verbose output, test execution, example running, and support for multiple CMake generators. This makes building the project much easier for developers on all platforms without needing to remember CMake commands.
- **Static library naming standardization**: Removed `_static` suffix from static library output names for cleaner, more conventional naming. Static libraries now generate as `libkstring.a` (Linux/macOS) and `kstring.lib` (Windows) instead of `libkstring_static.a`/`kstring_static.lib`. The target name remains `kstring_static` internally, but the OUTPUT_NAME property ensures clean output filenames. This aligns with standard library naming conventions where the file extension alone distinguishes static from shared libraries.

### September 25, 2025

- **Memory safety and code consistency enhancements**: Completed major refactoring of KString.c implementation with standardized KS_ prefix for all private functions, enhanced memory safety through KS_Release() function, comprehensive pointer validation, arithmetic overflow protection, and improved bounds checking. This strengthens the library's robustness while maintaining the 16-byte German String optimization specifications (commit bdb838d).
- **Copilot instructions confirmed and analyzed**: Reviewed complete project structure, confirmed understanding of Kraut Strings implementation based on German String research, and validated current codebase status. Project has solid foundation with complete API definition, partial implementation, working build system, and example program.

### September 18, 2025

- **Initial copilot instructions setup**: Added timestamp tracking and recent updates section to maintain change history and decision rationale for the KString library development process.
