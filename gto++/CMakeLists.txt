cmake_minimum_required(VERSION 3.16)
project(gto++ LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(external/phevaluator)

add_library(gto++ 
    src/action/action.cpp
    src/action/fixed_abstraction.cpp
    src/action/per_street_abstraction.cpp
    src/game/game_state.cpp
    src/solver/range.cpp
    src/solver/solver.cpp
    src/tree/info_set.cpp
    src/utils/utils.cpp
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
