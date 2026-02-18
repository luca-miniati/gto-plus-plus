cmake_minimum_required(VERSION 3.16)
project(gto++ LANGUAGES CXX)

# Use C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -----------------------
# Fetch Eigen 5.0.1
# -----------------------
include(FetchContent)

FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 5.0.1  # Latest stable release
)

FetchContent_MakeAvailable(eigen)

# -----------------------
# Add external libraries
# -----------------------
add_subdirectory(external/phevaluator)

# -----------------------
# Add gto++ library
# -----------------------
add_library(gto++
    src/action/action.cpp
    src/action/fixed_abstraction.cpp
    src/action/per_street_abstraction.cpp
    src/game/game_state.cpp
    src/game/game_model.cpp
    src/solver/range.cpp
    src/solver/solver.cpp
    src/info_set/info_set.cpp
    src/info_set/canonical_suit_abstraction.cpp
    src/tree/node.cpp
    src/tree/tree.cpp
    src/utils/utils.cpp
)

# Include directories
target_include_directories(gto++ PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/phevaluator/include
)

# Link libraries
target_link_libraries(gto++ PRIVATE pheval7 Eigen3::Eigen)

# -----------------------
# Add main executable
# -----------------------
add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE gto++)

# -----------------------
# Enable testing
# -----------------------
enable_testing()
add_subdirectory(tests)

