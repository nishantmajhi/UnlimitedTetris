# ================================================================
#  UnlimitedTetris — CMakeLists.txt
# ================================================================
#  The project previously had no build system at all. The only
#  build instructions were a comment in main.cpp naming a MinGW
#  installation under D:\Qt\Tools\, which exists on one computer.
#
#  Usage:
#      cmake -S . -B build
#      cmake --build build
#
#  If FreeGLUT lives somewhere CMake does not look by itself,
#  name the prefix rather than editing this file:
#      cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/freeglut
#
#  CMAKE_PREFIX_PATH, not GLUT_ROOT_PATH: on Windows FindGLUT
#  looks for the library only in <GLUT_ROOT_PATH>/Release, so a
#  normal lib/ + include/ prefix is not found that way.
# ================================================================

cmake_minimum_required(VERSION 3.16)
project(UnlimitedTetris LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(UnlimitedTetris
    main.cpp
    src/globals.cpp
    src/highscore.cpp
    src/unit2_scan.cpp
    src/unit3_transform.cpp
    src/unit4_3d.cpp
    src/unit5_curves.cpp
    src/unit8_lighting.cpp
    src/unit10_opengl.cpp
)

target_include_directories(UnlimitedTetris PRIVATE include)
target_link_libraries(UnlimitedTetris PRIVATE GLUT::GLUT OpenGL::GL)

# GLU is a separate library on Unix; on Windows it comes with the
# OpenGL import library that find_package(OpenGL) already found.
if(OPENGL_glu_LIBRARY)
    target_link_libraries(UnlimitedTetris PRIVATE ${OPENGL_glu_LIBRARY})
endif()

if(MSVC)
    target_compile_options(UnlimitedTetris PRIVATE /W4)
else()
    target_compile_options(UnlimitedTetris PRIVATE -Wall -Wextra)
endif()

# ── Tests ─────────────────────────────────────────────────────
#  The suite links no graphics library: tests/stubs/GL/glut.h
#  replaces the real header, and glstub.cpp records the drawing
#  instead of performing it. That include directory must come
#  first, which is what BEFORE does here.
enable_testing()

add_executable(unlimitedtetris_tests EXCLUDE_FROM_ALL
    tests/run.cpp
    tests/stubs/glstub.cpp
    src/globals.cpp
    src/highscore.cpp
    src/unit2_scan.cpp
    src/unit3_transform.cpp
    src/unit4_3d.cpp
    src/unit5_curves.cpp
    src/unit8_lighting.cpp
    src/unit10_opengl.cpp
)
target_include_directories(unlimitedtetris_tests BEFORE PRIVATE
    tests/stubs
    include
)

add_test(NAME rules COMMAND unlimitedtetris_tests)
