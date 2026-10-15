#ifndef UNIT5_CURVES_H
#define UNIT5_CURVES_H

/*
 * ================================================================
 *  UnlimitedTetris — include/unit5_curves.h
 *
 *  Unit 5: Parametric Curves
 *  ──────────────────────────
 *  5.2  Cubic Bezier — De Casteljau evaluation
 *       B(t) = (1-t)³P0 + 3(1-t)²tP1 + 3(1-t)t²P2 + t³P3
 *
 *  Used for:
 *    · Two decorative swirls on the side panel
 *    · (The Game Over screen sets up two more, but has never
 *      drawn them; the unused control points were removed.)
 *    · Two background curves on the Main Menu screen
 * ================================================================
 */

#include "gfx.h"
#include "globals.h"

struct V2 { float x, y; };

// Evaluate cubic Bezier at t ∈ [0,1]
V2 bezierPoint(V2 p0, V2 p1, V2 p2, V2 p3, float t);

// Draw the curve as a GL_LINE_STRIP with `n` segments
void drawBezier(V2 p0, V2 p1, V2 p2, V2 p3, int n,
                float r, float g, float b, float a = 1.0f);

#endif // UNIT5_CURVES_H
