# Code Review — UnlimitedTetris

**Remediated 2026-09-21 on branch `cleanup/2026-09-21`. Nothing pushed.**
The original review of 2026-09-04 is below, unchanged, under "First review".
This section is what was actually found and done.

---

## Corrections to the first review

Two of its findings do not survive checking, and both sent the reader away
from something real.

**"`README.md` describes the project structure accurately and lists real
features. No drift found." — wrong, and the drift was in the part a player
would hit first.** The controls table said <kbd>P</kbd> pauses and
<kbd>Esc</kbd> returns to the menu. The program pauses on <kbd>M</kbd> or
<kbd>Esc</kbd> (`src/unit10_opengl.cpp`, the keyboard callback) and has no key
that returns to the menu at all — that is an item inside the pause menu. The
game's own side panel prints `M    Pause`, so the README contradicted the
screen beside it. It also advertised "smooth piece-drop and rotation
animations", of which there are none — pieces move a whole cell at a time —
and linked to a `LICENSE` file that does not exist under a heading asserting
the MIT licence.

**"No debug output, no dead code" — wrong.** `renderGameOver()` declared eight
Bézier control points, `r0`–`r3` and `s0`–`s3`, for two ribbon curves that are
never drawn. `include/unit5_curves.h` listed those curves as a feature of the
Game Over screen. They were the only warnings in a `-Wall -Wextra` build, so
the first compiler to see this project named them immediately; no compiler had
seen it, which is the same reason the P1 below went unnoticed.

One finding was **understated rather than wrong**. Finding 4 said the high
score "is never saved" and "resets on every launch", and proposed either
persisting it or relabelling it "Best this session". Both are true, but the
score was also wrong *within* a session, on screen, which the review did not
notice — see below.

---

## What was wrong

### P1 — the Game Over screen could print a best score lower than the score above it

`highScore` was assigned in exactly three places: on a line clear, and at the
top of `startGame()` and `goMainMenu()`. Soft drops (`score++`) and hard drops
(`score+=2` per row) never compared against it.

A game can be scored entirely on drop points. Pieces spawn at `curX = COLS/2-2`
= 3 and are at most four cells wide, so columns 0-2 and 7-9 are never touched
and no row can ever complete. Hard-dropping every piece therefore tops the
stack out with a few hundred points and zero lines, and the screen at the end
reads:

```
Score: 412
Best:  0
```

The side panel's `BEST` reads `0` for that whole game too. The number was not
lost — it was applied at the start of the *next* game, one screen too late to
be believed. That is the library_system lesson from this archive again: a
figure with a label of its own that nobody had produced a value in.

Fixed by routing every point through `addScore()`, which updates the record in
the same breath, so `BEST` cannot trail `SCORE`. Proven by
`tests/original_test.cpp`, which asserts the wrong pair of numbers against the
first commit, and by the matching check in `tests/run.cpp`.

### P1 (from the first review) — no build system, and build instructions naming one machine

Confirmed exactly as written. `main.cpp`'s header comment and `temp.txt` both
carried `-I D:\Qt\Tools\mingw1310_64\include`. There is now a `CMakeLists.txt`
and a `Makefile`, both of which were run here and both of which build the game
and the tests.

The toolchain question the first review could not answer is answered:
**this project is buildable on this machine.**
`D:\strawberry-perl-5.42.0.1-64bit-portable\c\` is a MinGW-w64 13.2 that
includes `GL/glut.h` and `libglut.a`, so no install was needed. The game
builds clean under `-Wall -Wextra` and opens its window.

### P2 — the high score was never written down

Three screens advertise a best score and all three meant "best since this
window opened". `src/highscore.cpp` now keeps `highscore.txt` beside the
executable — beside the executable rather than in the working directory,
because a shortcut or a drag-and-drop launch would otherwise scatter records
around the disk.

### P2 — the scoring rule existed twice and the high-score comparison four times

Now one `awardLines()`, called by both the gravity tick and the hard drop. It
still multiplies by the level as it was when the rows were cleared, not as it
becomes afterwards, which is what the original did.

### Two smaller things found on the way

`goMainMenu()` reset `level` to 1 but left `dropInterval` at whatever the last
game had accelerated it to. Nothing observable came of it because
`startGame()` set it again, but the two lines disagreed; they now agree.

Reaching Game Over did not reset `menuItem`. Every route that could get there
happened to leave it at 0, so the bug was latent rather than live, but it
depended on a coincidence across three functions. `endGame()` now sets it.

---

## What was deliberately not changed

**Piece selection is still `rand() % PIECE_COUNT`.** The first review listed a
seven-bag randomiser as P3 and was careful to call it "a gameplay-fidelity
choice, not a defect". That is exactly right, and it is the reason it was left
alone: the standing instruction for this archive is that business logic is
fair game and the UX is not, and the only thing a seven-bag changes is how the
game feels to play. It is about ten lines in `spawnPiece()` whenever the author
wants it. The README now says the game works this way rather than leaving a
player to wonder.

**`draw3DBanner()` has a colour bug that was left in place.**
`src/unit4_3d.cpp` builds its ambient face with
`glColor3f(cg*.45f, cg*.45f, cb*.45f)` — `cg` twice, where the red channel is
meant. The comment above it says "Draw only the cyan rectangle" while the code
takes `rects[1]`, which is the purple one, and `rects[0]` and `rects[2]` are
never used. All of that is real, and all of it changes only what colour
appears on screen, so it is a UI change and out of scope here. It is written
down so the next reader does not have to rediscover it.

**No `LICENSE` file was added.** Choosing a licence is the author's decision.
The README no longer claims MIT, since nothing in the repository supported
that claim.

**The rotation system was not touched.** It kicks one column right then one
left and refuses otherwise, which is less than SRS allows. That is how the
game has always played; it is noted in the README's limitations.

---

## Tests

```sh
make test
```

or, on Windows, `powershell -ExecutionPolicy Bypass -File tests\run.ps1`,
which runs both suites.

**227 checks against the working tree, 22 against the first commit.** No
framework and no dependency, matching the project's own.

The technique worth stealing from this project: **`tests/stubs/GL/glut.h` is a
fake `<GL/glut.h>`**, placed first on the include path, so every real source
file compiles unmodified against a recorder that writes down the drawing
instead of performing it. Replacing the *header* rather than the functions is
the only way to do this on Windows — the genuine declarations are
`__declspec(dllimport)`, so defining `glVertex2f` locally is a compile error,
not an override. It is about 130 lines of declarations and 200 of recorder.

What that buys is the ability to call the real `display()` and read back the
text the panel printed. Every assertion about `SCORE`, `BEST`, `LINES` and
`LEVEL` is made against the characters that reach the screen, not the variable
behind them — which is the entire point, because in the P1 above the variable
was right and the screen was wrong.

`tests/run.ps1` builds a second binary from the first commit, fetched with
`git show` into a scratch directory. Nothing is transcribed and no copy is
kept in the repository, so those assertions cannot quietly stop describing the
original. That is available to any project here whose original code has no
hard dependency on its surroundings, and it is cheaper than the transcription
patterns used elsewhere in this archive.

The suite caught one bug in the fix: a lost backslash made the Windows branch
of `executableDir()` search for `"\/"`, which the compiler reads as `"/"`
alone, so the directory never resolved and every score would have been written
to the working directory instead. The check that caught it is the dullest one
in the file.

---

## Still needs a person

**Nothing here is blocking, and none of it is a fix to confirm — all three are
checks that the automated suite cannot reach.** About ten minutes in total.

1. **Play a game and close the window.** `highscore.txt` should appear beside
   the executable containing the score. Re-launch: the main menu should read
   `Best: <that number>` instead of "Be the first to set the score!". This is
   the only part of the persistence that no test covers — `loadHighScore` and
   `saveHighScore` are tested thoroughly, but the three lines in `main()` that
   wire them to the game are not, because a second `main` cannot be linked
   into the suite.

2. **Check the pause menu and the Game Over menu on a real screen.** The
   callbacks are tested through the recorder, but nobody has seen the menus
   since the change. Expected: identical to before.

3. **Confirm the record survives a quit through the menu.** Score some points,
   pause, choose Exit, relaunch, and check the best is still there. The logic
   is tested; the file being written at that exact moment is not.

Note that closing the window with the title-bar X during a game still loses a
record set in that game. GLUT gives the program no say in it, and that is
documented in the README rather than worked around.

---

## Note on two deleted files

`UnlimitedTetris.exe` and `freeglut.dll` were removed from the working tree,
as the first review's cleanup list asked. Both were gitignored, so neither was
ever in version control — which also means **`freeglut.dll` cannot be restored
from this repository.** It was an ordinary redistributable from a FreeGLUT
package and the README now says where to get one, but it is worth knowing that
it is gone rather than untracked. The executable rebuilds from source in a
second.

---
---

# First review

*Everything below is the original review of 2026-09-04, left as it was
written. Its action checklist is answered above.*

Reviewed 2026-09-04 against the working tree. Scope: `main.cpp`, `include/globals.h`,
`src/globals.cpp`, `src/unit10_opengl.cpp`, `README.md`, `temp.txt`.

## What this project is

A Tetris clone in C++ with FreeGLUT and fixed-function OpenGL. The file naming — `unit2_scan`,
`unit3_transform`, `unit4_3d`, `unit5_curves`, `unit8_lighting`, `unit10_opengl` — makes clear this
doubles as a computer-graphics coursework showcase, with each unit of the syllabus getting its own
translation unit.

## Verdict

The game logic is sound and the module split is a smart way to satisfy a syllabus without ending up
with one thousand-line file. The drop timer reschedules itself correctly (one registration, one
reschedule, no stacking — the classic GLUT timer bug is absent here). Collision, ghost piece, line
clearing and level progression are all present and structured sensibly.

What is missing is any way for someone else to build it, and the scoring logic is copy-pasted into
four places.

## Issues

**1. There is no build system, and the only build instructions hardcode your machine.**
Both the header comment in `main.cpp:11-13` and the scratch file `temp.txt` contain:

```
-I D:\Qt\Tools\mingw1310_64\include -L D:\Qt\Tools\mingw1310_64\lib
```

That path exists on exactly one computer. Anyone else — including you on a new machine — gets a
compiler error on the first `#include <GL/glut.h>`. Add a `CMakeLists.txt` (twenty lines: `find_package(OpenGL)`,
`find_package(GLUT)`, one `add_executable` listing the sources) or a `Makefile` with the include path
as an overridable variable. The Linux build line in the header is portable and correct — it is only
the Windows one that is machine-specific.

**2. `temp.txt` is a leftover scratch file** holding the same build command as the header comment.
It is gitignored, so it was never committed, but it is exactly the kind of development residue worth
sweeping. Delete it once the build system exists.

**3. Scoring is duplicated in four places.** `score += 100*level*cl*cl; lines += cl; updateLevel();
if (score > highScore) highScore = score;` appears at `src/unit10_opengl.cpp:238-240`, again at
`:261`, and the high-score comparison alone appears twice more in `src/globals.cpp:121` and `:130`.
Any change to the scoring formula has to be made in all of them, and they will eventually disagree.
Extract a single `awardLines(int cl)` function in `globals.cpp` and call it from both the soft-drop
and hard-drop paths.

**4. The high score is never saved.** `highScore` is an `int` initialised to 0 in `globals.cpp:53`
with no file I/O anywhere. The UI shows "Best:" on the menu, on the sidebar, and on the game-over
screen, which strongly implies persistence — but it resets on every launch. Either write it to a
small file next to the executable on game over, or relabel it "Best this session".

**5. Piece selection is uniform random rather than a 7-bag.** `nextType = rand() % PIECE_COUNT`
means genuine droughts are possible — modern Tetris uses a shuffled bag of all seven pieces to
guarantee each appears once per cycle. This is a gameplay-fidelity choice, not a defect, but it is the
single change that would most improve how the game feels, and it is about ten lines.

## Smaller notes

- `srand(time(nullptr))` plus `rand()` is fine for a game; `<random>` with `std::mt19937` would be
  the modern form if you revisit it.
- The board is a fixed `int board[ROWS][COLS]` with `ROWS`/`COLS` as compile-time constants and a
  parallel `boardColor` array. Clean and appropriate — no dynamic allocation anywhere, so no leak
  surface.
- Everything is a global with `extern` declarations in `globals.h`. For a GLUT program with C-style
  callbacks that is the conventional shape, and the header groups them clearly. Worth knowing that if
  you ever want two boards on screen, this is the thing that blocks it.
- Fixed-function OpenGL (`glBegin`-era) is deprecated but entirely correct for a course covering
  transformations and lighting at the primitive level. No change needed.
- `README.md` describes the project structure accurately and lists real features. No drift found.

## Cleanup

- `UnlimitedTetris.exe` and `freeglut.dll` sit in the working tree. Both are gitignored, so nothing is
  committed — delete them from disk, and ship `freeglut.dll` alongside a release build instead.
- No `LICENSE` file.
- No debug output, no dead code, no secrets, no trackers in the source.

## Action checklist

- [x] **P1** Add a `CMakeLists.txt` or `Makefile`; remove the hardcoded `D:\Qt\Tools\...` paths.
- [x] **P2** Extract the scoring update into one function; call it from both drop paths.
- [x] **P2** Persist the high score, or relabel it as session-only. *(Persisted, and the
      display bug behind it fixed — see P1 above.)*
- [ ] **P3** Switch to a 7-bag randomiser. *(Deliberately not done: it changes how the game
      plays. See "What was deliberately not changed".)*
- [x] **P3** Delete `temp.txt` and the stray binaries; ~~add a `LICENSE`~~ *(the licence is the
      author's decision)*.

## Notes for a future session

Build with the Linux line from the `main.cpp` header on a Unix box, or fix the Windows path first.
There are no tests, and the parts worth testing are pure: `collision`, `clearLines`, `ghostY` and
`updateLevel` in `globals.cpp` take only the board state and return answers, so they can be exercised
without a window. That is where to start if you ever want a regression net.

## Licence, 2026-10-02

The author chose one licence for all of their projects: free for personal,
non-commercial use, modifiable for the user's own use, and not to be
distributed, original or modified. No standard licence says that, so `LICENSE`
is a short custom text, the same in every project apart from the year and what
it excludes. The README's licence section says the same. Anything above that
calls the licence an open question, or calls the project MIT, is superseded.

## CI, 2026-10-02

`.github/workflows/ci.yml` runs `tests\run.ps1` under Windows PowerShell on
`windows-latest`, with full history for the first-commit half. Nothing has
been pushed, so it has not run on GitHub yet. Every job was run before
committing, from a fresh clone of this branch, the way a runner would run it:
Windows jobs here, in PowerShell 7 or the shell the step names, with only the
tools the job sets up on `PATH`. 227 and 22.
