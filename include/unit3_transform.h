#ifndef UNIT3_TRANSFORM_H
#define UNIT3_TRANSFORM_H

/*
 * ================================================================
 *  UnlimitedTetris — include/unit3_transform.h
 *
 *  Unit 3: 2D Geometric Transformations & Viewing
 *  ───────────────────────────────────────────────
 *  3.1  Translation     → board col/row offset to screen pixel
 *  3.2  Homogeneous     → glMatrixMode / glLoadIdentity stack
 *  3.3  Viewing pipeline→ set2D() via glOrtho
 *  3.4  Clipping        → collision() boundary test in globals
 * ================================================================
 */

#include "gfx.h"
#include "globals.h"

// 3.3  Set pixel-perfect 2D orthographic projection
//      Call at the start of every display() frame.
void set2D();

// 3.3  reshape callback — locks window to WIN_W × WIN_H
void reshape(int w, int h);

#endif // UNIT3_TRANSFORM_H
