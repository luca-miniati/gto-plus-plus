cmake_policy(SET CMP0135 NEW)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/32c3dce34bb0d3174a728ff8f9dbb51653b17e1e.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(test_action action/test_action.cpp)
add_executable(test_game_state game/test_game_state.cpp)
add_executable(test_game_model game/test_game_model.cpp)
target_link_libraries(test_action PRIVATE gto++ gtest_main)
target_link_libraries(test_game_state PRIVATE gto++ gtest_main)
target_link_libraries(test_game_model PRIVATE gto++ gtest_main)
target_include_directories(test_action PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(test_game_state PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(test_game_model PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

include(GoogleTest)
gtest_discover_tests(test_action)
gtest_discover_tests(test_game_state)
gtest_discover_tests(test_game_model)
