##############################################################################
#
# MIT License
#
# Copyright (c) 2025 Heiko Panjas
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
##############################################################################

cmake_minimum_required(VERSION 3.30)

# Project name and version
project(KString VERSION 0.1.0 LANGUAGES C)

# Use standard installation directories
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Set C standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add compiler flags for better error checking
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source files
set(KSTRING_SOURCES
    src/KString.c
)

# Header files
set(KSTRING_HEADERS
    include/KString.h
)

# Library type controlled by BUILD_SHARED_LIBS (default: shared)
option(BUILD_SHARED_LIBS "Build shared library instead of static" ON)

add_library(kstring ${KSTRING_SOURCES} ${KSTRING_HEADERS})

# Add alias target for FetchContent usage
add_library(KString::kstring ALIAS kstring)

# Set library properties
set_target_properties(kstring PROPERTIES
    VERSION ${PROJECT_VERSION}
    PUBLIC_HEADER "${KSTRING_HEADERS}"
    C_STANDARD 17
    C_STANDARD_REQUIRED ON
)

if(BUILD_SHARED_LIBS)
    set_target_properties(kstring PROPERTIES SOVERSION 1)
endif()

target_include_directories(kstring
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Installation rules
install(TARGETS kstring
    EXPORT KStringTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export targets for use in build tree
export(EXPORT KStringTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/KStringTargets.cmake"
    NAMESPACE KString::
)

# Install the export set
install(EXPORT KStringTargets
    FILE KStringTargets.cmake
    NAMESPACE KString::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/KString
)

# Generate package config file
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/KStringConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KStringConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/KString
)

# Generate version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/KStringConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install package config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KStringConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KStringConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/KString
)

# Generate pkg-config file
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/kstring.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/kstring.pc"
    @ONLY
)

# Install pkg-config file
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/kstring.pc"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

# Optional: Build examples
option(KSTRING_BUILD_EXAMPLES "Build example programs" ON)
if(KSTRING_BUILD_EXAMPLES)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/_examples")
        add_subdirectory(_examples)
    endif()
endif()
