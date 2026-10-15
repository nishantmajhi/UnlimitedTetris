/*
 * ================================================================
 *  UnlimitedTetris — src/unit4_3d.cpp
 *
 *  Unit 4: 3D Geometric Transformations & Viewing
 *  ────────────────────────────────────────────────
 *  4.1  3D Rotation     → glRotatef(goAngle, 0,1,0)
 *  4.2  Composite 3D    → glPushMatrix / glPopMatrix chain
 *  4.3  Perspective     → gluPerspective(45°, aspect, 0.1, 100)
 *       Camera          → gluLookAt(eye=0,0,5 → origin)
 *       Orthographic    → set2D() for all 2D rendering (contrast)
 * ================================================================
 */

#include "../include/unit4_3d.h"
#include "../include/unit3_transform.h"

// ── 4.1 / 4.2 / 4.3  3D Game Over Banner ─────────────────────
void draw3DBanner()
{
    // 4.2  Save current 2D ortho projection onto the matrix stack
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    // 4.3  Switch to perspective projection
    gluPerspective(45.0, (double)WIN_W/WIN_H, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);  glPushMatrix(); glLoadIdentity();
    // 4.3  Position camera
    gluLookAt(0,0,5, 0,0,0, 0,1,0);
    glEnable(GL_DEPTH_TEST);

    // 4.1  Y-axis rotation — the key 3D transform
    glRotatef(goAngle, 0, 1, 0);

    // Three wide rectangles — clean, minimal, rotating
    // Colors: cyan  |  purple  |  orange
    float rects[3][3] = {
        {0.0f, 0.82f, 0.88f},   // cyan
        {0.58f,0.18f, 0.88f},   // purple
        {0.92f,0.48f, 0.08f}    // orange
    };
    float rw = 0.72f, rh = 0.44f;

    // Draw only the cyan rectangle, centered at origin
    float bx = -rw * 0.5f;
    float cr = rects[1][0], cg = rects[1][1], cb = rects[1][2];

    // Ambient face (Ka=0.45)
    glColor3f(cg*.45f, cg*.45f, cb*.45f);
    glBegin(GL_QUADS);
    glVertex3f(bx,      -rh*.5f, 0); glVertex3f(bx+rw, -rh*.5f, 0);
    glVertex3f(bx+rw,    rh*.5f, 0); glVertex3f(bx,     rh*.5f, 0);
    glEnd();
    // Diffuse face (Kd=0.9)
    glColor3f(cr, cg, cb);
    glBegin(GL_QUADS);
    glVertex3f(bx+.025f, -rh*.5f+.025f, .001f); glVertex3f(bx+rw, -rh*.5f+.025f, .001f);
    glVertex3f(bx+rw,     rh*.5f,        .001f); glVertex3f(bx+.025f, rh*.5f,     .001f);
    glEnd();
    // Shadow outline
    glColor3f(cr*.22f, cg*.22f, cb*.22f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(bx,    -rh*.5f, 0); glVertex3f(bx+rw, -rh*.5f, 0);
    glVertex3f(bx+rw,  rh*.5f, 0); glVertex3f(bx,     rh*.5f, 0);
    glEnd();

    // 4.2  Restore saved matrices
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);  glPopMatrix();
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    set2D();  // back to 2D
}

// ── Timer — ~60 fps rotation ──────────────────────────────────
void timerAngle(int)
{
    if(gState==STATE_OVER){
        goAngle += 1.2f;
        if(goAngle >= 360.0f) goAngle -= 360.0f;
        glutPostRedisplay();
    }
    glutTimerFunc(16, timerAngle, 0);
}
