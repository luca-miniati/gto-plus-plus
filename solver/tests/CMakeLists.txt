cmake_policy(SET CMP0135 NEW)

# -----------------------
# Fetch GoogleTest
# -----------------------
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# -----------------------
# Find all test source files
# -----------------------
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# -----------------------
# Add test executables
# -----------------------
add_executable(gto-plus-plus-tests ${TEST_SOURCES})
target_link_libraries(gto-plus-plus-tests PUBLIC gto++ gtest_main)
include(GoogleTest)
gtest_discover_tests(gto-plus-plus-tests)