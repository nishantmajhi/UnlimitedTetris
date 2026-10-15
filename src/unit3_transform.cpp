/*
 * ================================================================
 *  UnlimitedTetris — src/unit3_transform.cpp
 *
 *  Unit 3: 2D Geometric Transformations & Viewing
 *  ───────────────────────────────────────────────
 *  3.1  Translation  → board (col,row) to screen pixel
 *       screen_x = col * CELL
 *       screen_y = (ROWS-1-row) * CELL   (Y-axis flip)
 *  3.2  Homogeneous  → glMatrixMode / glLoadIdentity
 *  3.3  Viewing      → glOrtho pixel-perfect mapping
 *  3.4  Clipping     → collision() in globals.cpp
 * ================================================================
 */

#include "../include/unit3_transform.h"

// ── 3.3  2D orthographic projection ──────────────────────────
//  Maps world rectangle [0,WIN_W]×[0,WIN_H] → NDC → viewport.
//  glOrtho(l,r,b,t,near,far): y=0 at bottom, y=WIN_H at top.
//  Called at the start of every frame to guarantee clean state.
void set2D()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIN_W, 0, WIN_H, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
}

// ── 3.3  Reshape callback — window-to-viewport + lock size ────
void reshape(int w, int h)
{
    if(w != WIN_W || h != WIN_H){
        glutReshapeWindow(WIN_W, WIN_H);
        return;
    }
    glViewport(0, 0, WIN_W, WIN_H);
    set2D();
}
