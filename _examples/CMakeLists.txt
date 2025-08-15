cmake_minimum_required(VERSION 4.0)

# Example executable (C only)
add_executable(kstring_demo main.c)

# Link against the kstring library
target_link_libraries(kstring_demo kstring)

# Include the KString headers
target_include_directories(kstring_demo PRIVATE ${CMAKE_SOURCE_DIR}/include)
