/*
 * ================================================================
 *  UnlimitedTetris — tests/run.cpp
 *
 *  The regression suite. Built by tests/run.ps1 against the real
 *  source files, with tests/stubs first on the include path so
 *  that OpenGL, GLU and GLUT resolve to the recorder in
 *  tests/stubs/glstub.cpp instead of to a window.
 *
 *  Two halves:
 *    · the rules of the game, called directly — globals.cpp needs
 *      no graphics library at all;
 *    · whole frames, by calling the real display() and reading
 *      back the text the panel and the menus printed. A number on
 *      a screen is not believed here until it has been produced.
 *
 *  The companion suite tests/original_test.cpp runs the same kind
 *  of checks against the baseline commit and asserts that the two
 *  defects are present there.
 * ================================================================
 */

#include "harness.h"
#include "stubs/glrecord.h"

#include "../include/globals.h"
#include "../include/highscore.h"
#include "../include/unit10_opengl.h"
#include "../include/unit5_curves.h"

#include <cstdio>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace {

// ── Fixtures ──────────────────────────────────────────────────

void clearBoard()
{
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c) { board[r][c] = 0; boardColor[r][c] = 0; }
}

// A game in a known state, with no randomness left in it.
void freshGame()
{
    clearBoard();
    gState = STATE_PLAY;
    score = lines = 0; level = 1; dropInterval = 500;
    highScore = 0; menuItem = 0; goAngle = 0;
    curType = 1; curRot = 0; curX = 3; curY = 0;   // O piece, columns 4-5
    nextType = 1;
}

// Fill row `r` completely except for the two named columns.
void fillRowExcept(int r, int gapA, int gapB)
{
    for (int c = 0; c < COLS; ++c)
        if (c != gapA && c != gapB) { board[r][c] = 1; boardColor[r][c] = 4; }
}

int filledCells()
{
    int n = 0;
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c) if (board[r][c]) ++n;
    return n;
}

typedef std::set<std::pair<int,int> > CellSet;

// The cells of one rotation as a set, shifted so the top-left of
// its bounding box is the origin. Two shapes compare equal under
// this if they are the same shape in a different place.
CellSet normalised(int piece, int rot)
{
    int minC = 99, minR = 99;
    for (int i = 0; i < 4; ++i) {
        if (SHAPES[piece][rot][i][0] < minC) minC = SHAPES[piece][rot][i][0];
        if (SHAPES[piece][rot][i][1] < minR) minR = SHAPES[piece][rot][i][1];
    }
    CellSet out;
    for (int i = 0; i < 4; ++i)
        out.insert(std::make_pair(SHAPES[piece][rot][i][0] - minC,
                                  SHAPES[piece][rot][i][1] - minR));
    return out;
}

// That same set turned a quarter turn clockwise: (c,r) -> (maxR-r, c).
CellSet turnedCW(const CellSet& in)
{
    int maxR = 0;
    for (CellSet::const_iterator it = in.begin(); it != in.end(); ++it)
        if (it->second > maxR) maxR = it->second;

    CellSet out;
    for (CellSet::const_iterator it = in.begin(); it != in.end(); ++it)
        out.insert(std::make_pair(maxR - it->second, it->first));
    return out;
}

// ── Recorder for the persistence hook ─────────────────────────
std::vector<int> g_persisted;
void recordPersist(int value) { g_persisted.push_back(value); }

std::string tempPath(const char* leaf)
{
    const char* dir = getenv("TEMP");
    if (!dir || !*dir) dir = getenv("TMPDIR");
    if (!dir || !*dir) dir = ".";
    return std::string(dir) + "/ut_" + leaf;
}

void removeFile(const std::string& path) { remove(path.c_str()); }

} // namespace

int main()
{
    ut::begin("UnlimitedTetris — regression suite");
    onHighScorePersist = recordPersist;

    // ============================================================
    ut::section("the tetromino table");
    {
        bool fourDistinct = true, inRange = true;
        for (int p = 0; p < PIECE_COUNT; ++p)
            for (int rt = 0; rt < ROT_COUNT; ++rt) {
                CellSet cells;
                for (int i = 0; i < 4; ++i) {
                    int c = SHAPES[p][rt][i][0], r = SHAPES[p][rt][i][1];
                    if (c < 0 || c > 3 || r < 0 || r > 3) inRange = false;
                    cells.insert(std::make_pair(c, r));
                }
                if (cells.size() != 4) fourDistinct = false;
            }
        ut::check(fourDistinct, "every rotation of every piece is four distinct cells");
        ut::check(inRange,      "every cell sits inside the 4x4 spawn box");

        bool rotationsAgree = true;
        for (int p = 0; p < PIECE_COUNT; ++p)
            for (int rt = 0; rt < ROT_COUNT; ++rt)
                if (turnedCW(normalised(p, rt)) != normalised(p, (rt + 1) % ROT_COUNT))
                    rotationsAgree = false;
        ut::check(rotationsAgree,
                  "each rotation really is the previous one turned 90 degrees");

        bool coloursSane = true;
        for (int p = 0; p < PIECE_COUNT; ++p)
            for (int ch = 0; ch < 3; ++ch)
                if (COLORS[p][ch] < 0.0f || COLORS[p][ch] > 1.0f) coloursSane = false;
        ut::check(coloursSane, "every piece colour is inside the 0..1 range");
    }

    // ============================================================
    ut::section("collision");
    {
        freshGame();
        ut::check(!collision(1, 0, 3, 0), "an empty board accepts the spawn position");
        ut::check(collision(1, 0, -2, 0), "a piece cannot sit off the left edge");
        ut::check(collision(1, 0, COLS - 1, 0), "nor off the right edge");
        ut::check(collision(1, 0, 3, ROWS - 1), "nor through the floor");
        ut::check(collision(1, 0, 3, -1), "nor above the ceiling");
        board[5][4] = 1;
        ut::check(collision(1, 0, 3, 4), "nor through a locked cell");
        ut::check(!collision(1, 0, 0, 4), "but beside one is fine");
        // The I piece spans four columns, so it is the one that can
        // reach both walls: rotation 0 occupies x..x+3.
        ut::check(!collision(0, 0, 0, 5), "the I piece fits flush against the left wall");
        ut::check(!collision(0, 0, COLS - 4, 5), "and flush against the right wall");
        ut::check(collision(0, 0, COLS - 3, 5), "but not one column further");
    }

    // ============================================================
    ut::section("line clearing");
    {
        freshGame();
        ut::eqInt(clearLines(), 0, "an empty board clears nothing");

        freshGame();
        fillRowExcept(ROWS - 1, 4, 5);
        ut::eqInt(clearLines(), 0, "a row with a gap in it is not full");
        ut::eqInt(filledCells(), COLS - 2, "and nothing is removed");

        freshGame();
        for (int c = 0; c < COLS; ++c) { board[ROWS-1][c] = 1; boardColor[ROWS-1][c] = 3; }
        board[ROWS-2][0] = 1; boardColor[ROWS-2][0] = 6;
        ut::eqInt(clearLines(), 1, "a full row clears");
        ut::eqInt(filledCells(), 1, "leaving only what was above it");
        ut::eqInt(board[ROWS-1][0], 1, "which has fallen by one row");
        ut::eqInt(boardColor[ROWS-1][0], 6, "keeping its colour");

        freshGame();
        for (int r = ROWS - 4; r < ROWS; ++r)
            for (int c = 0; c < COLS; ++c) board[r][c] = 1;
        ut::eqInt(clearLines(), 4, "four full rows clear together");
        ut::eqInt(filledCells(), 0, "and the board is empty afterwards");

        freshGame();
        for (int c = 0; c < COLS; ++c) { board[ROWS-1][c] = 1; board[ROWS-3][c] = 1; }
        board[ROWS-2][2] = 1; boardColor[ROWS-2][2] = 5;
        ut::eqInt(clearLines(), 2, "two full rows with a gapped row between them clear");
        ut::eqInt(filledCells(), 1, "the row between survives");
        ut::eqInt(board[ROWS-1][2], 1, "and lands on the floor");
        ut::eqInt(boardColor[ROWS-1][2], 5, "with its colour intact");

        freshGame();
        for (int c = 0; c < COLS; ++c) board[0][c] = 1;
        ut::eqInt(clearLines(), 1, "a full row at the very top clears");
        ut::eqInt(filledCells(), 0, "and leaves nothing behind");
    }

    // ============================================================
    ut::section("locking a piece");
    {
        freshGame();
        curType = 1; curRot = 0; curX = 3; curY = ROWS - 2;
        lockPiece();
        ut::eqInt(filledCells(), 4, "an O piece locks four cells");
        ut::eqInt(board[ROWS-2][4], 1, "at the columns it occupied");
        ut::eqInt(boardColor[ROWS-1][5], 1, "recording which piece it was");
    }

    // ============================================================
    ut::section("the ghost piece");
    {
        freshGame();
        curType = 1; curRot = 0; curX = 3; curY = 0;
        ut::eqInt(ghostY(), ROWS - 2, "the ghost falls to the floor on an empty board");

        for (int c = 0; c < COLS; ++c) board[ROWS-1][c] = 1;
        ut::eqInt(ghostY(), ROWS - 3, "and rests on top of a stack");

        curY = ROWS - 3;
        ut::eqInt(ghostY(), curY, "a piece already resting has its ghost underneath it");
    }

    // ============================================================
    ut::section("levels and gravity");
    {
        freshGame();
        lines = 0;   updateLevel();
        ut::eqInt(level, 1, "no lines is level 1");
        ut::eqInt(dropInterval, 500, "level 1 drops at 500ms");
        lines = 9;   updateLevel(); ut::eqInt(level, 1, "nine lines is still level 1");
        lines = 10;  updateLevel(); ut::eqInt(level, 2, "ten lines is level 2");
        ut::eqInt(dropInterval, 460, "level 2 drops at 460ms");
        lines = 25;  updateLevel(); ut::eqInt(level, 3, "25 lines is level 3");
        ut::eqInt(dropInterval, 420, "level 3 drops at 420ms");
        lines = 110; updateLevel(); ut::eqInt(level, 12, "110 lines is level 12");
        ut::eqInt(dropInterval, 60, "level 12 is the fastest, 60ms");
        lines = 5000; updateLevel();
        ut::eqInt(dropInterval, 60, "and it never drops below 60ms");
        ut::check(dropInterval > 0, "so the timer interval is never zero or negative");
    }

    // ============================================================
    ut::section("scoring");
    {
        freshGame();
        awardLines(0);
        ut::eqInt(score, 0, "clearing nothing scores nothing");
        ut::eqInt(lines, 0, "and counts no lines");

        freshGame(); awardLines(1); ut::eqInt(score, 100,  "one line at level 1 scores 100");
        freshGame(); awardLines(2); ut::eqInt(score, 400,  "two lines score 400");
        freshGame(); awardLines(3); ut::eqInt(score, 900,  "three lines score 900");
        freshGame(); awardLines(4); ut::eqInt(score, 1600, "four lines score 1600");

        freshGame();
        level = 3;
        awardLines(4);
        ut::eqInt(score, 4800, "and the level multiplies it");

        // The level is applied as it was when the rows were cleared,
        // not as it becomes afterwards.
        freshGame();
        lines = 9;
        awardLines(1);
        ut::eqInt(score, 100, "the tenth line scores at level 1");
        ut::eqInt(level, 2,   "and takes the player to level 2");

        freshGame();
        lines = 4;
        awardLines(2);
        ut::eqInt(lines, 6, "cleared rows are added to the count");
    }

    // ============================================================
    ut::section("the best score follows the score, as it happens");
    {
        freshGame();
        for (int i = 0; i < 12; ++i) softDrop();
        ut::check(score > 0, "soft drops score");
        ut::eqInt(highScore, score, "and the best keeps up with them");

        freshGame();
        hardDrop();
        ut::check(score > 0, "a hard drop scores");
        ut::eqInt(highScore, score, "and the best keeps up with that too");

        freshGame();
        highScore = 10000;
        hardDrop();
        ut::eqInt(highScore, 10000, "a score below the record leaves it alone");

        freshGame();
        awardLines(4);
        ut::eqInt(highScore, 1600, "a line clear updates the best");
    }

    // ============================================================
    ut::section("REGRESSION: a game won on drop points alone records its best");
    {
        // The defect this suite exists for. Pieces spawn at column 3
        // and are at most four wide, so columns 0-2 and 7-9 stay
        // empty and no line can ever complete: the whole score comes
        // from hard drops. The original committed the best only on a
        // line clear or at the start of the next game, so a game like
        // this ended on a screen reading "Score: 412" and "Best:  0"
        // at the same time.
        clearBoard();
        score = lines = 0; level = 1; highScore = 0; dropInterval = 500;
        gState = STATE_PLAY; nextType = 1; spawnPiece();
        g_persisted.clear();

        int guard = 0;
        while (gState == STATE_PLAY && guard++ < 500) hardDrop();

        ut::eqInt(gState, STATE_OVER, "the stack tops out and the game ends");
        ut::eqInt(lines, 0,           "with no line ever cleared");
        ut::check(score > 0,          "but a score on the board");
        ut::eqInt(highScore, score,   "the best equals the score that earned it");
        ut::check(!g_persisted.empty(), "and the record was written down");
        if (!g_persisted.empty())
            ut::eqInt(g_persisted.back(), score, "with the right value");

        gl_reset();
        display();
        std::string shown = gl_allText();
        ut::contains(shown, "Score: " + std::to_string(score),
                     "the Game Over screen prints the final score");
        ut::contains(shown, "Best:  " + std::to_string(score),
                     "and prints the same number as the best");
        ut::eqStr(gl_textAfter("BEST"), std::to_string(score),
                  "the side panel agrees");
    }

    // ============================================================
    ut::section("moving and rotating");
    {
        freshGame();
        curType = 1; curX = 4; curY = 5;
        moveSideways(-1); ut::eqInt(curX, 3, "left moves left");
        moveSideways(+1); ut::eqInt(curX, 4, "right moves right");

        curX = -1;                       // O piece occupies columns 0-1
        moveSideways(-1); ut::eqInt(curX, -1, "the wall stops it");

        freshGame();
        curType = 0; curRot = 1; curX = 4; curY = 5;   // vertical I
        rotateCW();
        ut::eqInt(curRot, 2, "a rotation with room simply happens");
        ut::eqInt(curX, 4,   "without moving the piece");

        // Against the right wall the horizontal form does not fit
        // where the vertical one stood, so it kicks one column across.
        freshGame();
        curType = 0; curRot = 1; curX = COLS - 3; curY = 5;
        rotateCW();
        ut::eqInt(curRot, 2, "against a wall it still rotates");
        ut::eqInt(curX, COLS - 4, "by kicking sideways");

        // Boxed in on both sides: the rotation is refused rather
        // than being forced through a locked cell.
        freshGame();
        curType = 0; curRot = 0; curX = 3; curY = 5;   // horizontal I
        for (int r = 0; r < ROWS; ++r)
            for (int c = 0; c < COLS; ++c)
                if (r != 5 && r != 6) board[r][c] = 1;
        rotateCW();
        ut::eqInt(curRot, 0, "a rotation with nowhere to go is refused");
        ut::check(!collision(curType, curRot, curX, curY),
                  "and the piece is left somewhere legal");
    }

    // ============================================================
    ut::section("dropping");
    {
        freshGame();
        curType = 1; curX = 3; curY = 0;
        softDrop();
        ut::eqInt(curY, 1, "a soft drop falls one row");
        ut::eqInt(score, 1, "and scores one point");

        freshGame();
        curType = 1; curX = 3; curY = 0;
        hardDrop();
        ut::eqInt(score, 2 * (ROWS - 2), "a hard drop scores two points a row");
        ut::eqInt(filledCells(), 4, "and locks the piece where it landed");

        // A hard drop that completes rows scores for the drop and
        // for the lines, and leaves a new piece in play.
        freshGame();
        fillRowExcept(ROWS - 1, 4, 5);
        fillRowExcept(ROWS - 2, 4, 5);
        curType = 1; curRot = 0; curX = 3; curY = 0;
        nextType = 1;
        hardDrop();
        ut::eqInt(lines, 2, "the drop completed both rows");
        ut::eqInt(filledCells(), 0, "which left the board clear");
        ut::eqInt(level, 1, "two lines is not yet a new level");
        ut::check(score >= 400, "and it scored at least the line award");
        ut::eqInt(gState, STATE_PLAY, "the game carries on");
        ut::eqInt(curY, 0, "with a fresh piece at the top");

        freshGame();
        curType = 1; curX = 3; curY = ROWS - 2;
        hardDrop();
        ut::eqInt(score, 0, "a piece already resting scores nothing for dropping");
    }

    // ============================================================
    ut::section("gravity");
    {
        freshGame();
        curType = 1; curX = 3; curY = 0;
        stepGravity();
        ut::eqInt(curY, 1, "one tick falls one row");
        ut::eqInt(score, 0, "gravity is not worth points");

        freshGame();
        curType = 1; curX = 3; curY = ROWS - 2; nextType = 1;
        stepGravity();
        ut::eqInt(filledCells(), 4, "a piece that cannot fall locks");
        ut::eqInt(curY, 0, "and the next one starts at the top");

        freshGame();
        gState = STATE_PAUSE;
        curType = 1; curX = 3; curY = 4;
        stepGravity();
        ut::eqInt(curY, 4, "a paused game does not fall");
        gState = STATE_OVER;
        stepGravity();
        ut::eqInt(curY, 4, "and nor does a finished one");
    }

    // ============================================================
    ut::section("spawning and the end of the game");
    {
        freshGame();
        nextType = 4;
        spawnPiece();
        ut::eqInt(curType, 4, "the piece that was next becomes current");
        ut::eqInt(curRot, 0,  "upright");
        ut::eqInt(curX, COLS/2 - 2, "in the middle");
        ut::eqInt(curY, 0,    "at the top");
        ut::eqInt(gState, STATE_PLAY, "and the game continues");

        freshGame();
        g_persisted.clear();
        addScore(750);
        for (int c = 0; c < COLS; ++c) { board[0][c] = 1; board[1][c] = 1; }
        nextType = 1;
        spawnPiece();
        ut::eqInt(gState, STATE_OVER, "a piece with nowhere to spawn ends the game");
        ut::eqInt(menuItem, 0, "with the menu on its first item");
        ut::check(!g_persisted.empty(), "and the record written down");
        if (!g_persisted.empty())
            ut::eqInt(g_persisted.back(), 750, "at the value just reached");
    }

    // ============================================================
    ut::section("starting and leaving a game");
    {
        freshGame();
        addScore(500);
        for (int c = 0; c < COLS; ++c) board[ROWS-1][c] = 1;
        lines = 14; updateLevel();
        startGame();
        ut::eqInt(filledCells(), 0, "a new game clears the board");
        ut::eqInt(score, 0,   "resets the score");
        ut::eqInt(lines, 0,   "resets the lines");
        ut::eqInt(level, 1,   "resets the level");
        ut::eqInt(dropInterval, 500, "resets the speed");
        ut::eqInt(highScore, 500, "and keeps the best");
        ut::eqInt(gState, STATE_PLAY, "and starts playing");

        freshGame();
        addScore(320);
        lines = 30; updateLevel();
        goMainMenu();
        ut::eqInt(gState, STATE_MENU, "leaving a game returns to the menu");
        ut::eqInt(score, 0, "with the score cleared");
        ut::eqInt(highScore, 320, "and the best kept");
        ut::eqInt(level, 1, "the level back to 1");
        ut::eqInt(dropInterval, 500, "and the speed back with it");
    }

    // ============================================================
    ut::section("the menu state machine");
    {
        gState = STATE_MENU; menuItem = 0;
        ut::eqInt(menuCount(), 2, "the main menu has two items");
        menuMove(+1); ut::eqInt(menuItem, 1, "down moves down");
        menuMove(+1); ut::eqInt(menuItem, 0, "and wraps at the bottom");
        menuMove(-1); ut::eqInt(menuItem, 1, "up wraps at the top");

        gState = STATE_PAUSE; menuItem = 0;
        ut::eqInt(menuCount(), 3, "the pause menu has three items");
        menuMove(-1); ut::eqInt(menuItem, 2, "and wraps the same way");

        gState = STATE_OVER; menuItem = 0;
        ut::eqInt(menuCount(), 3, "so does the game over menu");

        gState = STATE_PLAY; menuItem = 0;
        ut::eqInt(menuCount(), 0, "there is no menu while playing");
        menuMove(+1);
        ut::eqInt(menuItem, 0, "so the arrows cannot move one");

        gState = STATE_MENU; menuItem = 0;
        ut::eqInt(menuActivate(), MENU_ACTION_HANDLED, "New Game is handled here");
        ut::eqInt(gState, STATE_PLAY, "and starts a game");

        gState = STATE_MENU; menuItem = 1;
        ut::eqInt(menuActivate(), MENU_ACTION_QUIT, "Exit asks the caller to quit");

        freshGame();
        gState = STATE_PAUSE; menuItem = 0;
        ut::eqInt(menuActivate(), MENU_ACTION_HANDLED, "Continue is handled");
        ut::eqInt(gState, STATE_PLAY, "and resumes the same game");

        freshGame();
        addScore(90);
        gState = STATE_PAUSE; menuItem = 1;
        menuActivate();
        ut::eqInt(gState, STATE_PLAY, "New Game from the pause menu restarts");
        ut::eqInt(score, 0, "with a fresh score");
        ut::eqInt(highScore, 90, "and the best carried over");

        freshGame();
        gState = STATE_OVER; menuItem = 0;
        ut::eqInt(menuActivate(), MENU_ACTION_HANDLED, "Play Again is handled");
        ut::eqInt(gState, STATE_PLAY, "and starts another game");

        freshGame();
        gState = STATE_OVER; menuItem = 1;
        ut::eqInt(menuActivate(), MENU_ACTION_HANDLED, "Main Menu is handled");
        ut::eqInt(gState, STATE_MENU, "and goes to the menu");

        gState = STATE_OVER; menuItem = 2;
        ut::eqInt(menuActivate(), MENU_ACTION_QUIT, "Exit from Game Over quits");

        freshGame();
        addScore(640);
        gState = STATE_PAUSE; menuItem = 2;
        g_persisted.clear();
        menuActivate();
        ut::check(!g_persisted.empty(),
                  "quitting mid-game writes the record down first");
        if (!g_persisted.empty())
            ut::eqInt(g_persisted.back(), 640, "at the value reached");
    }

    // ============================================================
    ut::section("the keyboard");
    {
        freshGame();
        curType = 1; curX = 3; curY = 0;
        keyboard(' ', 0, 0);
        ut::eqInt(filledCells(), 4, "space hard-drops and locks");

        freshGame();
        keyboard('m', 0, 0);
        ut::eqInt(gState, STATE_PAUSE, "m pauses");
        ut::eqInt(menuItem, 0, "on the first menu item");

        freshGame();
        keyboard('M', 0, 0);
        ut::eqInt(gState, STATE_PAUSE, "and so does a capital M");

        freshGame();
        keyboard(27, 0, 0);
        ut::eqInt(gState, STATE_PAUSE, "escape pauses too");

        freshGame();
        gState = STATE_OVER;
        keyboard('m', 0, 0);
        ut::eqInt(gState, STATE_OVER, "but not once the game is over");

        freshGame();
        gState = STATE_PAUSE;
        curType = 1; curX = 3; curY = 4;
        keyboard(' ', 0, 0);
        ut::eqInt(curY, 4, "a paused game ignores the drop key");
        ut::eqInt(filledCells(), 0, "and nothing locks");

        freshGame();
        gState = STATE_MENU;
        curType = 1; curX = 3; curY = 4;
        keyboard(' ', 0, 0);
        ut::eqInt(filledCells(), 0, "and so does the main menu");

        freshGame();
        gState = STATE_PAUSE; menuItem = 0;
        keyboard(13, 0, 0);
        ut::eqInt(gState, STATE_PLAY, "enter activates the highlighted item");

        freshGame();
        curType = 1; curX = 3; curY = 4;
        keyboard('q', 0, 0);
        ut::eqInt(curY, 4, "an unused key does nothing");
        ut::eqInt(gState, STATE_PLAY, "and does not change the state");
    }

    // ============================================================
    ut::section("the arrow keys");
    {
        freshGame();
        curType = 1; curX = 4; curY = 5;
        specialKeys(GLUT_KEY_LEFT, 0, 0);  ut::eqInt(curX, 3, "left arrow moves left");
        specialKeys(GLUT_KEY_RIGHT, 0, 0); ut::eqInt(curX, 4, "right arrow moves right");
        specialKeys(GLUT_KEY_DOWN, 0, 0);  ut::eqInt(curY, 6, "down arrow soft-drops");
        ut::eqInt(score, 1, "scoring one point");
        specialKeys(GLUT_KEY_UP, 0, 0);    ut::eqInt(curRot, 1, "up arrow rotates");

        freshGame();
        gState = STATE_PAUSE; menuItem = 0;
        specialKeys(GLUT_KEY_DOWN, 0, 0);
        ut::eqInt(menuItem, 1, "in a menu, down moves the selection");
        ut::eqInt(curY, 0, "and does not touch the piece");

        freshGame();
        gState = STATE_OVER; menuItem = 0;
        specialKeys(GLUT_KEY_UP, 0, 0);
        ut::eqInt(menuItem, 2, "and up wraps to the last item");

        freshGame();
        gState = STATE_MENU; menuItem = 0;
        specialKeys(GLUT_KEY_RIGHT, 0, 0);
        ut::eqInt(gState, STATE_PLAY, "right activates the selection in a menu");
    }

    // ============================================================
    ut::section("the gravity timer never stacks");
    {
        freshGame();
        for (int i = 0; i < 5; ++i) {
            gl_reset();
            timerDrop(0);
            ut::eqInt((long long)gl_timers().size(), 1,
                      "each firing schedules exactly one successor");
            ut::eqInt((long long)gl_timers()[0].msecs, dropInterval,
                      "at the current drop interval");
            ut::eqInt(gl_redisplays(), 1, "and asks for exactly one redraw");
        }

        gState = STATE_MENU;
        gl_reset();
        timerDrop(0);
        ut::eqInt((long long)gl_timers().size(), 1,
                  "the timer keeps running while the menu is up");
    }

    // ============================================================
    ut::section("what a frame actually draws");
    {
        freshGame();
        score = 1234; lines = 7; level = 3; highScore = 9999;
        gl_reset();
        display();
        ut::eqInt(gl_swaps(), 1, "a frame is presented once");
        ut::eqStr(gl_textAfter("SCORE"), "1234", "the panel prints the score");
        ut::eqStr(gl_textAfter("BEST"),  "9999", "the best");
        ut::eqStr(gl_textAfter("LINES"), "7",    "the lines");
        ut::eqStr(gl_textAfter("LEVEL"), "3",    "and the level");
        ut::contains(gl_allText(), "Hard drop", "the controls are listed");

        // One quad per locked cell, four for the falling piece and
        // four for its ghost, on top of the screen's furniture.
        freshGame();
        curType = 1; curRot = 0; curX = 3; curY = 0;
        gl_reset(); display();
        int emptyBoard = gl_countPrims(GL_QUADS);
        board[ROWS-1][0] = 1;
        gl_reset(); display();
        ut::eqInt(gl_countPrims(GL_QUADS), emptyBoard + 1,
                  "one more locked cell draws one more quad");

        // The ghost is suppressed when it would sit exactly under
        // the piece, so a resting piece is not drawn twice.
        freshGame();
        curType = 1; curRot = 0; curX = 3; curY = 0;
        gl_reset(); display();
        int withGhost = gl_countPrims(GL_QUADS);
        curY = ROWS - 2;
        gl_reset(); display();
        ut::eqInt(gl_countPrims(GL_QUADS), withGhost - 4,
                  "a piece on the floor draws no ghost");

        freshGame();
        gState = STATE_MENU; highScore = 0;
        gl_reset(); display();
        ut::contains(gl_allText(), "Be the first to set the score!",
                     "a machine with no record says so");
        highScore = 4242;
        gl_reset(); display();
        ut::contains(gl_allText(), "Best: 4242", "and otherwise shows the record");
        ut::contains(gl_allText(), "New Game", "the main menu offers a new game");

        freshGame();
        gState = STATE_PAUSE;
        gl_reset(); display();
        ut::contains(gl_allText(), "PAUSED", "the pause screen says so");
        ut::contains(gl_allText(), "Continue", "and offers to continue");

        freshGame();
        score = 77; highScore = 77;
        gState = STATE_OVER;
        gl_reset(); display();
        ut::contains(gl_allText(), "GAME  OVER", "the game over screen says so");
        ut::contains(gl_allText(), "Score: 77", "prints the score");
        ut::contains(gl_allText(), "Best:  77", "and a best that is not below it");
    }

    // ============================================================
    ut::section("the Bezier curves behind the menus");
    {
        V2 p0 = {0,0}, p1 = {0,100}, p2 = {100,100}, p3 = {100,0};
        V2 at0 = bezierPoint(p0,p1,p2,p3,0.0f);
        V2 at1 = bezierPoint(p0,p1,p2,p3,1.0f);
        ut::nearFloat(at0.x, 0.0,   "the curve starts at its first control point");
        ut::nearFloat(at0.y, 0.0,   "in both axes");
        ut::nearFloat(at1.x, 100.0, "and ends at its last");
        ut::nearFloat(at1.y, 0.0,   "in both axes");
        V2 mid = bezierPoint(p0,p1,p2,p3,0.5f);
        ut::nearFloat(mid.x, 50.0, "a symmetric curve is halfway across at t=0.5");
        ut::nearFloat(mid.y, 75.0, "and three quarters of the way up");

        gl_reset();
        drawBezier(p0,p1,p2,p3,8,1,1,1);
        ut::eqInt(gl_countPrims(GL_LINE_STRIP), 1, "it draws one line strip");
        ut::eqInt((long long)gl_prims()[0].verts.size(), 9,
                  "with n+1 points for n segments");
    }

    // ============================================================
    ut::section("reading a high-score file");
    {
        ut::eqInt(parseHighScore("4200"),      4200, "a plain number reads back");
        ut::eqInt(parseHighScore("  4200  "),  4200, "surrounding whitespace is fine");
        ut::eqInt(parseHighScore("4200\n"),    4200, "so is a trailing newline");
        ut::eqInt(parseHighScore("0"),         0,    "zero is a score");
        ut::eqInt(parseHighScore(""),          0,    "an empty file is no record");
        ut::eqInt(parseHighScore("   "),       0,    "nor is whitespace");
        ut::eqInt(parseHighScore("-5"),        0,    "a negative number is not a score");
        ut::eqInt(parseHighScore("12ab"),      0,    "nor is a number with junk after it");
        ut::eqInt(parseHighScore("abc"),       0,    "nor a word");
        ut::eqInt(parseHighScore("12 34"),     0,    "nor two numbers");
        ut::eqInt(parseHighScore("99999999999999999999"), 2147483647,
                  "an absurd number clamps instead of wrapping negative");

        ut::eqStr(formatHighScore(4200), "4200\n", "writing adds a newline");
        ut::eqStr(formatHighScore(0),    "0\n",    "zero writes as zero");
        ut::eqStr(formatHighScore(-9),   "0\n",    "a negative value writes as zero");
    }

    // ============================================================
    ut::section("writing a high-score file");
    {
        std::string path = tempPath("highscore_test.txt");
        removeFile(path);
        ut::eqInt(loadHighScore(path), 0, "a missing file is no record");

        ut::check(saveHighScore(path, 5150), "a score writes");
        ut::eqInt(loadHighScore(path), 5150, "and reads back");

        ut::check(saveHighScore(path, 6000), "a better score overwrites");
        ut::eqInt(loadHighScore(path), 6000, "and reads back");

        FILE* f = fopen(path.c_str(), "wb");
        if (f) { fputs("not a score at all", f); fclose(f); }
        ut::eqInt(loadHighScore(path), 0, "a corrupt file reads as no record");

        f = fopen(path.c_str(), "wb");
        if (f) { for (int i = 0; i < 200; ++i) fputc('7', f); fclose(f); }
        ut::eqInt(loadHighScore(path), 0, "and so does an absurdly long one");

        removeFile(path);
        ut::check(!saveHighScore(tempPath("no_such_dir/there/highscore.txt"), 1),
                  "a path that cannot be written reports failure");

        ut::check(highScorePath().size() > std::string("highscore.txt").size(),
                  "the live path is qualified by the executable's directory");
        ut::contains(highScorePath(), "highscore.txt", "and names the score file");
    }

    return ut::report();
}
