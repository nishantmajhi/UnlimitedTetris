/*
 * ================================================================
 *  UnlimitedTetris — src/unit8_lighting.cpp
 *
 *  Unit 8: Illumination Models & Surface Rendering
 *  ─────────────────────────────────────────────────
 *  8.1  Phong model per cell:
 *         Ambient  Ka=0.55  → base face fill
 *         Diffuse  Kd=0.90  → bright top-left bevel (N·L≈0.9)
 *         Specular Ks=0.40  → white GL_TRIANGLE_FAN dot
 *  8.2  Attenuation         → dark bottom-right shadow bevel ×0.25
 *  8.3  Constant shading    → ghost cells (ambient only)
 *       Phong shading       → active + locked cells (all terms)
 * ================================================================
 */

#include "../include/unit8_lighting.h"

float clamp1(float v){ return v<0?0:v>1?1:v; }

// ── 8.1 / 8.3  Full Phong cell ───────────────────────────────
void drawCell(int col, int row, float r, float g, float b, float a)
{
    // Unit 3.1: board-to-screen translation (row 0 = top → high Y)
    float x0 = (float)(col * CELL);
    float y0 = (float)((ROWS-1-row) * CELL);
    float x1 = x0+CELL-1.0f, y1 = y0+CELL-1.0f;

    // Ambient fill  Ka=0.55
    glColor4f(r*.55f, g*.55f, b*.55f, a);
    glBegin(GL_QUADS);
    glVertex2f(x0,y0); glVertex2f(x1,y0);
    glVertex2f(x1,y1); glVertex2f(x0,y1);
    glEnd();

    // Diffuse bevel — bright top-left  Kd=0.9, N·L≈0.9
    glColor4f(clamp1(r*1.5f), clamp1(g*1.5f), clamp1(b*1.5f), a);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(x0,y0); glVertex2f(x0,y1);  // left edge
    glVertex2f(x0,y1); glVertex2f(x1,y1);  // top edge
    glEnd();

    // Attenuation / shadow — dark bottom-right  ×0.25
    glColor4f(r*.25f, g*.25f, b*.25f, a);
    glBegin(GL_LINES);
    glVertex2f(x1,y0); glVertex2f(x1,y1);  // right edge
    glVertex2f(x0,y0); glVertex2f(x1,y0);  // bottom edge
    glEnd();

    // Specular dot  Ks=0.40 — white GL_TRIANGLE_FAN circle
    float sx=x0+CELL*.24f, sy=y0+CELL*.72f, sr=CELL*.08f;
    glColor4f(1,1,1, a*.4f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(sx, sy);
    for(int i=0;i<=12;i++){
        float ang = i*6.2832f/12;
        glVertex2f(sx+sr*cosf(ang), sy+sr*sinf(ang));
    }
    glEnd();
}

// ── 8.3  Ghost cell — constant ambient only ───────────────────
void drawGhost(int col, int row, float r, float g, float b)
{
    float x0=(float)(col*CELL), y0=(float)((ROWS-1-row)*CELL);
    glColor4f(r*.18f, g*.18f, b*.18f, .45f);
    glBegin(GL_QUADS);
    glVertex2f(x0,y0);      glVertex2f(x0+CELL-1,y0);
    glVertex2f(x0+CELL-1,y0+CELL-1); glVertex2f(x0,y0+CELL-1);
    glEnd();
}

// ── Next-piece preview cell (20×20 px with bevel) ─────────────
void drawPreview(float cx, float cy, int t)
{
    float r=COLORS[t][0], g=COLORS[t][1], b=COLORS[t][2];
    glColor3f(r,g,b);
    glBegin(GL_QUADS);
    glVertex2f(cx,cy); glVertex2f(cx+20,cy);
    glVertex2f(cx+20,cy+20); glVertex2f(cx,cy+20);
    glEnd();
    glColor3f(clamp1(r*1.5f),clamp1(g*1.5f),clamp1(b*1.5f));
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex2f(cx,cy+20); glVertex2f(cx,cy);
    glVertex2f(cx,cy+20); glVertex2f(cx+20,cy+20);
    glEnd();
}
