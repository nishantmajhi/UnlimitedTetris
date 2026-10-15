/*
 * ================================================================
 *  UnlimitedTetris — main.cpp
 *  Entry point: GLUT initialisation and callback registration.
 * ================================================================
 *  Building is described in README.md. In short, from this
 *  directory:
 *
 *    cmake -S . -B build && cmake --build build
 *
 *  or, with no CMake to hand:
 *
 *    make                       (see Makefile for the variables)
 * ================================================================
 */

#include "include/gfx.h"
#include "include/globals.h"
#include "include/highscore.h"
#include "include/unit3_transform.h"
#include "include/unit4_3d.h"
#include "include/unit10_opengl.h"

// The game logic decides *when* the record is worth writing down;
// this is the half that knows how. globals.cpp calls it through a
// function pointer so that it never has to mention a file.
static void writeHighScoreToDisk(int value)
{
    saveHighScore(highScorePath(), value);
}

int main(int argc, char** argv)
{
    srand((unsigned)time(nullptr));

    highScore = loadHighScore(highScorePath());
    onHighScorePersist = writeHighScoreToDisk;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WIN_W, WIN_H);
    glutInitWindowPosition(120, 80);
    glutCreateWindow("UnlimitedTetris | OpenGL");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutDisplayFunc (display);
    glutReshapeFunc (reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc (specialKeys);

    gState   = STATE_MENU;
    nextType = rand() % PIECE_COUNT;

    glutTimerFunc(500, timerDrop,  0);
    glutTimerFunc(16,  timerAngle, 0);

    glutMainLoop();
    return 0;
}
