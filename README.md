# UnlimitedTetris

A Tetris clone in C++ and fixed-function OpenGL, drawn through FreeGLUT.
There is no engine and no framework underneath it: the board is an array, the
grid lines are a Bresenham loop, the cell highlights are a Phong model
evaluated by hand, and the curves behind the menus are cubic Béziers
evaluated per segment.

The file layout says what the project really is. `unit2_scan`,
`unit3_transform`, `unit4_3d`, `unit5_curves`, `unit8_lighting` and
`unit10_opengl` are the units of a computer-graphics syllabus, one
translation unit each, and the game is the thing they are demonstrated on.
That is a good structure for the purpose and it has been kept.

---

## Building

You need a C++17 compiler, OpenGL (which comes with your graphics drivers)
and FreeGLUT.

### With CMake

```sh
cmake -S . -B build
cmake --build build
```

If FreeGLUT is somewhere CMake does not look on its own, name the prefix it
lives under — the directory with `include/GL/glut.h` and `lib/libglut.a`
inside it:

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/freeglut
```

Use `CMAKE_PREFIX_PATH` rather than `GLUT_ROOT_PATH`: on Windows, CMake's
`FindGLUT` looks for the library only in `<GLUT_ROOT_PATH>/Release`, so an
ordinary `lib/` + `include/` prefix is not found that way.

### With make

```sh
make            # the game
make test       # build and run the test suite
make clean
```

Both include and library paths are variables, so a FreeGLUT in an unusual
place needs no edit to the Makefile:

```sh
make GL_INC=-IC:/freeglut/include GL_LIB=-LC:/freeglut/lib
```

### By hand

```sh
g++ -std=c++17 -Iinclude main.cpp src/*.cpp -o UnlimitedTetris -lglut -lGL -lGLU
```

On Windows the last three become `-lglut -lopengl32 -lglu32`, and some
FreeGLUT packages name their import library `freeglut` rather than `glut`.

### Getting FreeGLUT

| Platform | How |
| --- | --- |
| Debian / Ubuntu | `sudo apt install g++ freeglut3-dev libglu1-mesa-dev` |
| Fedora | `sudo dnf install gcc-c++ freeglut-devel mesa-libGLU-devel` |
| Arch | `sudo pacman -S gcc freeglut glu` |
| macOS | `brew install freeglut` — the system GLUT framework also works but is deprecated |
| Windows | Pre-built MinGW binaries from [transmissionzero.co.uk](https://www.transmissionzero.co.uk/software/freeglut-devel/), or any MinGW-w64 distribution that ships one |

On Windows the built executable needs FreeGLUT's DLL at run time, either
beside the `.exe` or on `PATH`. A build that links `libglut.a` from a MinGW
distribution wants *that* distribution's DLL — the names vary between
packagings, so copy the one belonging to the import library you linked.

---

## Playing

| Key | Action |
| --- | --- |
| <kbd>←</kbd> <kbd>→</kbd> | Move left / right |
| <kbd>↑</kbd> | Rotate clockwise |
| <kbd>↓</kbd> | Soft drop — one row, one point |
| <kbd>Space</kbd> | Hard drop — straight down, two points a row |
| <kbd>M</kbd> or <kbd>Esc</kbd> | Pause |
| <kbd>↑</kbd> <kbd>↓</kbd> | Move the selection, in any menu |
| <kbd>Enter</kbd> or <kbd>→</kbd> | Choose the selected item |

There is no key that resumes a paused game directly; pausing opens a menu
whose first item is **Continue**.

### Scoring

Clearing rows scores `100 × level × rows²`, so four rows at once are worth
sixteen times one row and not four times: 1600 at level 1 against 100. The
level is the one the rows were cleared on, not the one they take you to.

The level is `lines / 10 + 1`. Each level takes 40ms off the interval between
gravity ticks, starting at 500ms and stopping at 60ms, which is level 12.

The best score is kept in `highscore.txt` beside the executable, and survives
closing the game. It is written when a game ends, when you leave a game, and
when you quit through a menu — but not if the window is closed with a game
still in progress.

---

## Layout

```
UnlimitedTetris/
├── CMakeLists.txt
├── Makefile
├── main.cpp                  GLUT setup and callback registration
├── include/
│   ├── gfx.h                 the one place OpenGL enters the program
│   ├── globals.h             the rules of the game — no graphics
│   ├── highscore.h
│   └── unit*.h
├── src/
│   ├── globals.cpp           collision, clearing, scoring, menu states
│   ├── highscore.cpp         reading and writing the record
│   └── unit*.cpp             the drawing
└── tests/
    ├── run.cpp               the regression suite
    ├── original_test.cpp     the same questions, asked of the first commit
    ├── harness.h
    ├── run.ps1               builds and runs both
    └── stubs/                a fake OpenGL that records instead of drawing
```

`globals.cpp` pulls in no graphics library at all. Everything that decides
something — whether a piece fits, which rows are full, what a drop is worth,
what a menu item does — lives there and can be called directly, which is what
makes the suite below possible.

---

## Tests

```sh
make test
```

or, on Windows:

```powershell
powershell -ExecutionPolicy Bypass -File tests\run.ps1
```

227 checks, no test framework and no dependency. They cover the shape table,
collision, line clearing, the ghost piece, scoring, levels, the menu state
machine, both keyboard handlers, the gravity timer and the high-score file.

The suite also draws. `tests/stubs/GL/glut.h` replaces the real header, so
the real source files compile unmodified against a recorder that writes down
what it was asked to draw instead of drawing it. A test can then call the
actual `display()` and read back the text the panel printed — which is how
the checks on the score, the best, the lines and the level are made against
the number that reaches the screen rather than the variable behind it.

`tests/run.ps1` also builds a second binary from the sources as they were in
this repository's first commit, fetched with `git show`, and asserts that the
defect fixed here is present in them. Nothing is transcribed and no copy is
kept, so those assertions cannot quietly stop describing the original. A test
that passes against the original is testing nothing.

---

## Known limitations

- Piece selection is a plain `rand() % 7` rather than the shuffled seven-bag
  modern Tetris uses, so long droughts of a particular piece are possible.
  This is how the game has always played and changing it would change how it
  feels, so it has been left alone deliberately.
- Rotation kicks one column left or right and no further, so some rotations
  that a full SRS implementation would allow are refused here.
- The window is a fixed 500×640 and resizing is rejected rather than scaled.
- Closing the window mid-game with the title bar loses a record set in that
  game; GLUT gives the program no say in it.

## Licence

Free for personal, non-commercial use. You may use, study and modify this
software for yourself, but you may not use it commercially, and you may not
share or publish it, in its original form or modified. See [LICENSE](LICENSE)
for the full terms.
