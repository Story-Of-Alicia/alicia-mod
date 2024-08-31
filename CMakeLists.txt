cmake_minimum_required(VERSION 3.20)
project(alicia_mod)

add_subdirectory(3rd-party)

add_library(alicia_mod SHARED
        src/main.cpp
        src/ui.cpp)
target_compile_features(alicia_mod PUBLIC cxx_std_20)
target_link_libraries(alicia_mod PRIVATE imgui d3d9)
