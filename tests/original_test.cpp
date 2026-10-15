/*
 * ================================================================
 *  UnlimitedTetris — tests/original_test.cpp
 *
 *  Driven against the ORIGINAL sources, fetched out of the
 *  baseline commit by tests/run.ps1 into a scratch directory and
 *  compiled there against the fake GL in tests/stubs. Nothing is
 *  transcribed and nothing is copied into the repository: the code
 *  under test here is byte-for-byte what `git show` returns, so
 *  these assertions cannot quietly stop describing the original.
 *
 *  Every check in this file asserts the behaviour of the UNFIXED
 *  program, including the two defects. It exists so that the
 *  regression tests in tests/run.cpp are known to be testing
 *  something: a test that passes against the original tests
 *  nothing.
 * ================================================================
 */

#include "harness.h"
#include "stubs/glrecord.h"

#include "../include/globals.h"
#include "../include/unit10_opengl.h"

#include <string>

namespace {

// Hard-drop pieces until the stack tops out. Pieces spawn at
// curX = COLS/2-2 = 3 and are at most four cells wide, so columns
// 0-2 and 7-9 are never touched and no line can ever complete —
// which is what makes this a game that scores without clearing.
int playUntilGameOver()
{
    int drops = 0;
    while (gState == STATE_PLAY && drops < 500) {
        keyboard(' ', 0, 0);
        ++drops;
    }
    return drops;
}

} // namespace

int main()
{
    ut::begin("UnlimitedTetris — original (baseline commit)");

    ut::section("the game scores without ever clearing a line");
    startGame();
    int drops = playUntilGameOver();
    ut::check(gState == STATE_OVER, "the stack tops out and the game ends");
    ut::check(drops < 500,          "it ends within the drop budget");
    ut::check(lines == 0,           "no line was ever completed");
    ut::check(score > 0,            "hard drops scored points");

    ut::section("DEFECT: the high score is not committed when the game ends");
    ut::eqInt(highScore, 0,
              "highScore is still zero after a game worth hundreds of points");
    ut::check(score > highScore,
              "the score beats the recorded best, and the best does not move");

    ut::section("DEFECT: the Game Over screen shows a Best below its own Score");
    gl_reset();
    display();
    std::string shown = gl_allText();
    ut::contains(shown, "Score: " + std::to_string(score),
                 "the Game Over screen prints the final score");
    ut::contains(shown, "Best:  0",
                 "and prints Best: 0 beside it");
    ut::eqStr(gl_textAfter("BEST"), "0",
              "the side panel's BEST reads 0 too");

    ut::section("the best only moves when the NEXT game starts");
    int finalScore = score;
    startGame();
    ut::eqInt(highScore, finalScore,
              "startGame() is what commits the previous game's score");
    ut::eqInt(score, 0, "and the score has already been reset by then");

    ut::section("a line clear is the only thing that updates BEST in play");
    startGame();
    // Fill the bottom two rows except columns 4 and 5, then drop an O
    // piece into the notch: it completes both rows at once.
    for (int r = ROWS - 2; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            if (c != 4 && c != 5) { board[r][c] = 1; boardColor[r][c] = 0; }
    curType = 1; curRot = 0; curX = 3; curY = 0;   // O occupies columns 4,5
    score = 0; highScore = 0; lines = 0;
    keyboard(' ', 0, 0);
    ut::eqInt(lines, 2, "the hard drop clears both rows");
    ut::check(score > 0, "and scores for them");
    ut::eqInt(highScore, score,
              "a line clear is the one path that does update BEST");

    ut::section("behaviour that must survive the fix");
    startGame();
    ut::eqInt(gState, STATE_PLAY, "startGame enters play");
    ut::eqInt(level, 1,           "level starts at 1");
    ut::eqInt(dropInterval, 500,  "drop interval starts at 500ms");

    gl_reset();
    timerDrop(0);
    ut::eqInt((long long)gl_timers().size(), 1,
              "one gravity tick reschedules exactly one timer");

    startGame();
    curType = 1; curRot = 0; curX = 4; curY = 5;   // O piece
    specialKeys(GLUT_KEY_LEFT, 0, 0);
    ut::eqInt(curX, 3, "left arrow moves the piece left");
    specialKeys(GLUT_KEY_RIGHT, 0, 0);
    ut::eqInt(curX, 4, "right arrow moves it back");

    lines = 25;
    updateLevel();
    ut::eqInt(level, 3, "25 lines is level 3");
    ut::eqInt(dropInterval, 420, "level 3 drops at 420ms");

    return ut::report();
}
