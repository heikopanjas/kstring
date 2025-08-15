cmake_minimum_required(VERSION 4.0)

# Project name and version
project(KString VERSION 1.0.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add compiler flags for better error checking
if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
endif()

# Include directories
include_directories(include)

# Source files
set(KSTRING_SOURCES
    src/KString.c
)

# Header files
set(KSTRING_HEADERS
    include/KString.h
)

# Create the library
add_library(kstring SHARED ${KSTRING_SOURCES} ${KSTRING_HEADERS})

# Set library properties
set_target_properties(kstring PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "include/KString.h"
)

# Platform-specific library naming (CMake handles this automatically)
# Linux: libkstring.so, macOS: libkstring.dylib, Windows: kstring.dll

# Installation rules
install(TARGETS kstring
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin  # For Windows DLLs
    PUBLIC_HEADER DESTINATION include
)

# Optional: Create a static library as well
add_library(kstring_static STATIC ${KSTRING_SOURCES} ${KSTRING_HEADERS})
set_target_properties(kstring_static PROPERTIES OUTPUT_NAME kstring_static)

# Optional: Enable testing if you want to add tests later
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    # Add test subdirectory if it exists
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests")
        add_subdirectory(tests)
    endif()
endif()

# Optional: Build examples
option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/_examples")
        add_subdirectory(_examples)
    endif()
endif()
