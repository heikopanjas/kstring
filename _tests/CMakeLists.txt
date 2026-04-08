##############################################################################
# KString Unit Tests
##############################################################################

# Unity test framework (vendored)
add_library(unity STATIC
    ${CMAKE_SOURCE_DIR}/_vendor/unity/unity.c
)
target_include_directories(unity PUBLIC
    ${CMAKE_SOURCE_DIR}/_vendor/unity
)

# Suppress warnings in vendored code
if(MSVC)
    target_compile_options(unity PRIVATE /W0)
else()
    target_compile_options(unity PRIVATE -w)
endif()

# Macro to define a test executable and register with CTest
function(kstring_add_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE kstring unity)
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

# Register all test executables
kstring_add_test(test_error              test_error.c)
kstring_add_test(test_create             test_create.c)
kstring_add_test(test_access             test_access.c)
kstring_add_test(test_memory             test_memory.c)
kstring_add_test(test_compare            test_compare.c)
kstring_add_test(test_compare_icase      test_compare_icase.c)
kstring_add_test(test_operations         test_operations.c)
kstring_add_test(test_encoding_convert   test_encoding_convert.c)
