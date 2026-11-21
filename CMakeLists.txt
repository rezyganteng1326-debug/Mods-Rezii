cmake_minimum_required(VERSION 3.18)
project(R3ZiMods)

add_library(R3ZiMods SHARED src/main.cpp)

target_link_libraries(R3ZiMods
    android
    log
)
