#ifndef UNIT10_OPENGL_H
#define UNIT10_OPENGL_H

/*
 * ================================================================
 *  UnlimitedTetris — include/unit10_opengl.h
 *
 *  Unit 10: Introduction to OpenGL
 *  ─────────────────────────────────
 *  10.1  Callback functions registered in main():
 *          display()     → glutDisplayFunc
 *          reshape()     → glutReshapeFunc   (unit3_transform)
 *          keyboard()    → glutKeyboardFunc
 *          specialKeys() → glutSpecialFunc
 *          timerDrop()   → glutTimerFunc  (gravity)
 *          timerAngle()  → glutTimerFunc  (3D anim, unit4_3d)
 *  10.1  Color commands: glColor3f / glColor4f
 *  10.1  Drawing: GL_POINTS, GL_LINES, GL_QUADS,
 *                 GL_LINE_STRIP, GL_TRIANGLE_FAN, GL_LINE_LOOP
 *  10.1  Double-buffer: GLUT_DOUBLE + glutSwapBuffers()
 *  10.1  Blend: glEnable(GL_BLEND) for transparency
 * ================================================================
 */

#include "gfx.h"
#include "globals.h"

// Screen renderers (called from display)
void renderMainMenu();
void renderBoard();
void renderPanel();
void renderPauseMenu();
void renderGameOver();

// Menu box — shared by all menu screens
void drawMenuBox(float cx, float cy, float bw, float bh,
                 const char** items, int cnt, int sel,
                 const char* title);

// GLUT callbacks
void display();
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void timerDrop(int val);

#endif // UNIT10_OPENGL_H
