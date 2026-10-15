/*
 * ================================================================
 *  UnlimitedTetris — src/unit5_curves.cpp
 *
 *  Unit 5: Parametric Curves
 *  ──────────────────────────
 *  5.2  Cubic Bezier via Bernstein polynomial form:
 *       B(t) = (1-t)³P0 + 3(1-t)²tP1 + 3(1-t)t²P2 + t³P3
 *       t ∈ [0,1], curve passes through P0 (t=0) and P3 (t=1).
 *       P1, P2 act as tangent-direction control handles.
 * ================================================================
 */

#include "../include/unit5_curves.h"

// ── 5.2  Evaluate cubic Bezier at parameter t ─────────────────
V2 bezierPoint(V2 p0, V2 p1, V2 p2, V2 p3, float t)
{
    float u = 1.0f - t;
    return {
        u*u*u*p0.x + 3*u*u*t*p1.x + 3*u*t*t*p2.x + t*t*t*p3.x,
        u*u*u*p0.y + 3*u*u*t*p1.y + 3*u*t*t*p2.y + t*t*t*p3.y
    };
}

// ── 5.2  Draw Bezier as GL_LINE_STRIP with n segments ─────────
void drawBezier(V2 p0, V2 p1, V2 p2, V2 p3, int n,
                float r, float g, float b, float a)
{
    glColor4f(r,g,b,a);
    glLineWidth(1.6f);
    glBegin(GL_LINE_STRIP);
    for(int i=0;i<=n;i++){
        V2 p = bezierPoint(p0,p1,p2,p3,(float)i/n);
        glVertex2f(p.x, p.y);
    }
    glEnd();
}
