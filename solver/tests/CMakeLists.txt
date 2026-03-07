cmake_policy(SET CMP0135 NEW)

# -----------------------
# Fetch GoogleTest
# -----------------------
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/32c3dce34bb0d3174a728ff8f9dbb51653b17e1e.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# -----------------------
# Find all test source files
# -----------------------
file(GLOB_RECURSE TEST_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# -----------------------
# Add test executables
# -----------------------
include(GoogleTest)

foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)

    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} PRIVATE gto++ gtest_main)

    # Use gtest_discover_tests with explicit CONFIGURATIONS and working directory
    gtest_discover_tests(${test_name}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        CONFIGURATIONS Debug Release RelWithDebInfo
    )
endforeach()