cmake_minimum_required(VERSION 3.13x)

project(GravSaverNG)

set(BUILD_SHARED_LIBS false)

add_subdirectory(SFML-2.5.1)

add_executable(gravsaverng
    src/gravsaverng.cpp
    src/gravsaverng.h
    src/rng.h
    src/stopwatch.h
)

target_link_libraries(gravsaverng PRIVATE
    sfml-graphics
    sfml-system
    sfml-window
)