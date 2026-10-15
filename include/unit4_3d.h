#ifndef UNIT4_3D_H
#define UNIT4_3D_H

/*
 * ================================================================
 *  UnlimitedTetris — include/unit4_3d.h
 *
 *  Unit 4: 3D Geometric Transformations & Viewing
 *  ────────────────────────────────────────────────
 *  4.1  3D Rotation        → glRotatef(goAngle, 0,1,0)
 *  4.2  Composite 3D       → glPushMatrix / glPopMatrix
 *  4.3  Perspective proj.  → gluPerspective + gluLookAt
 *       Contrasted with orthographic used for 2D board (Unit 3)
 *
 *  Rendered on the Game Over screen: rainbow "GAME OVER"
 *  billboard spins continuously on the Y-axis.
 * ================================================================
 */

#include "gfx.h"
#include "globals.h"

// Draw the 3D rotating "GAME OVER" banner (Unit 4.1 / 4.3)
void draw3DBanner();

// Timer — advances goAngle at ~60 fps
void timerAngle(int val);

#endif // UNIT4_3D_H
