# CMakeList.txt : CMake project for gltf-wizard, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (gltf-wizard "gltf-wizard.cpp" "gltf-wizard.h" "tiny_gltf.h" "json.hpp")

# TODO: Add tests and install targets if needed.
