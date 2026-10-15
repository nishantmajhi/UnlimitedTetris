# ================================================================
#  UnlimitedTetris — Makefile
# ================================================================
#  For machines with a compiler but no CMake. Every path that is
#  specific to an installation is a variable you can override on
#  the command line; nothing here names a directory on one
#  particular computer, which is what the build comment in
#  main.cpp used to do.
#
#      make                       build the game
#      make test                  build and run the test suite
#      make clean                 remove build output
#
#  Windows, with FreeGLUT somewhere of its own:
#      make GL_INC=-IC:/freeglut/include GL_LIB=-LC:/freeglut/lib
# ================================================================

# `CXX ?= g++` would not do what it looks like: make defines CXX
# itself, so ?= never fires and some make builds carry a default
# pointing at whatever compiler built them. Override only make's
# own default, leaving the environment and the command line free
# to say otherwise.
ifeq ($(origin CXX),default)
  CXX := g++
endif

CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra
GL_INC   ?=
GL_LIB   ?=

SRC = main.cpp \
      src/globals.cpp \
      src/highscore.cpp \
      src/unit2_scan.cpp \
      src/unit3_transform.cpp \
      src/unit4_3d.cpp \
      src/unit5_curves.cpp \
      src/unit8_lighting.cpp \
      src/unit10_opengl.cpp

# The rules of the game, with no drawing in them. This is the set
# the test suite links; none of it needs a graphics library.
LOGIC_SRC = src/globals.cpp src/highscore.cpp

DRAW_SRC = src/unit2_scan.cpp \
           src/unit3_transform.cpp \
           src/unit4_3d.cpp \
           src/unit5_curves.cpp \
           src/unit8_lighting.cpp \
           src/unit10_opengl.cpp

ifeq ($(OS),Windows_NT)
  EXE      = UnlimitedTetris.exe
  TEST_EXE = tests/run_tests.exe
  GL_LIBS  = -lglut -lopengl32 -lglu32
else
  EXE      = UnlimitedTetris
  TEST_EXE = tests/run_tests
  GL_LIBS  = -lglut -lGL -lGLU
endif

.PHONY: all test clean

all: $(EXE)

$(EXE): $(SRC)
	$(CXX) $(CXXFLAGS) -Iinclude $(GL_INC) $(SRC) -o $@ $(GL_LIB) $(GL_LIBS)

# tests/stubs comes first on the include path so that <GL/glut.h>
# resolves to the fake one. No OpenGL is linked.
test: $(TEST_EXE)
	./$(TEST_EXE)

$(TEST_EXE): tests/run.cpp tests/stubs/glstub.cpp $(LOGIC_SRC) $(DRAW_SRC)
	$(CXX) $(CXXFLAGS) -Itests/stubs -Iinclude \
	    tests/run.cpp tests/stubs/glstub.cpp $(LOGIC_SRC) $(DRAW_SRC) -o $@

clean:
	-rm -f $(EXE) $(TEST_EXE)
