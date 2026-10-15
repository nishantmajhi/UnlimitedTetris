#ifndef GLOBALS_H
#define GLOBALS_H

/*
 * ================================================================
 *  UnlimitedTetris — include/globals.h
 *  Shared constants, tetromino data, game state, core logic.
 *  Included by every other unit header.
 * ================================================================
 *  This header pulls in no graphics library. Everything declared
 *  below is a rule of the game rather than a way of drawing it,
 *  and globals.cpp links without OpenGL, GLUT or a window. The
 *  drawing units get OpenGL from gfx.h instead.
 * ================================================================
 */

#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>

// ── Window / board layout ─────────────────────────────────────
const int COLS    = 10;
const int ROWS    = 20;
const int CELL    = 32;
const int PANEL_W = 180;
const int WIN_W   = COLS * CELL + PANEL_W;  // 500
const int WIN_H   = ROWS * CELL;             // 640

// ── Tetromino table ───────────────────────────────────────────
const int PIECE_COUNT = 7;
const int ROT_COUNT   = 4;

extern const int   SHAPES[PIECE_COUNT][ROT_COUNT][4][2];
extern const float COLORS[PIECE_COUNT][3];

// ── Game state ────────────────────────────────────────────────
enum GameState { STATE_MENU, STATE_PLAY, STATE_PAUSE, STATE_OVER };

// What the caller of menuActivate() has to do about the choice.
// Returning this rather than calling exit() inside the logic is
// what makes the menu testable: a test cannot assert on a
// function that terminates the process.
enum MenuAction { MENU_ACTION_NONE, MENU_ACTION_HANDLED, MENU_ACTION_QUIT };

extern GameState gState;
extern int  menuItem;
extern int  board[ROWS][COLS];
extern int  boardColor[ROWS][COLS];
extern int  curType, curRot, curX, curY;
extern int  nextType;
extern int  score, lines, level, highScore;
extern int  dropInterval;
extern float goAngle;

// ── Core logic (defined in globals.cpp) ──────────────────────
bool collision(int tp, int rt, int x, int y);
void lockPiece();
int  clearLines();
int  ghostY();
void updateLevel();
void spawnPiece();
void startGame();
void goMainMenu();
void endGame();

// ── Scoring ───────────────────────────────────────────────────
//  Every point the game awards goes through addScore(), which is
//  also the only place the running best is updated. awardLines()
//  is the single copy of the line-clear rule; the gravity tick
//  and the hard drop both call it.
void addScore(int points);
void awardLines(int cleared);

// ── Player and timer actions ──────────────────────────────────
//  Each ignores input unless the game is being played, so the
//  GLUT callbacks do not have to guard them.
void stepGravity();
void softDrop();
void hardDrop();
void moveSideways(int dx);
void rotateCW();
void pauseGame();

// ── Menu state machine ────────────────────────────────────────
int        menuCount();
void       menuMove(int delta);
MenuAction menuActivate();

// ── High-score persistence hook ───────────────────────────────
//  globals.cpp holds the rule about when the record is worth
//  writing down; it does not know how to write it. main() points
//  this at the file writer in highscore.cpp, and the tests point
//  it at a recorder.
extern void (*onHighScorePersist)(int newHighScore);
void persistHighScore();

#endif // GLOBALS_H
