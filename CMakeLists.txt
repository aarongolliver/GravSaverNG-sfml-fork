cmake_minimum_required(VERSION 3.9x)

project(GravSaverNG)

set(BUILD_SHARED_LIBS false)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(SFML-2.5.1)
add_subdirectory(simplefilewatcher)

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
    simplefilewatcher
)

add_executable(shader_tester
    shader_tester/src/shader_tester.cpp
    shader_tester/src/shader_tester.h
    shader_tester/src/shader_header.h
)

target_include_directories(simplefilewatcher PRIVATE
    shader_tester/src/
)

target_link_libraries(shader_tester PRIVATE
    sfml-graphics
    sfml-system
    sfml-window
    simplefilewatcher
)