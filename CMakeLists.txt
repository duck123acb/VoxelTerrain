cmake_minimum_required(VERSION 3.30)
project(VoxelTerrain)

set(CMAKE_CXX_STANDARD 20)

# Add Raylib to the project
find_package(raylib REQUIRED)

add_executable(VoxelTerrain src/main.cpp)

# Link Raylib to the executable
target_link_libraries(VoxelTerrain raylib)
