cmake_minimum_required(VERSION 3.16)
project(gto++ LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(external/phevaluator)

add_library(gto++ 
    src/action.cpp
    src/game_state.cpp
    src/range.cpp
    src/solver.cpp
    src/action_abstraction/fixed_abstraction.cpp
    src/action_abstraction/per_street_abstraction.cpp
)
target_include_directories(gto++ PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/phevaluator/include
)
target_link_libraries(gto++ PRIVATE pheval7)

add_executable(main
  src/main.cpp
)
target_link_libraries(main PRIVATE gto++)
target_link_libraries(gto++ PRIVATE pheval7)

enable_testing()
add_subdirectory(tests)
