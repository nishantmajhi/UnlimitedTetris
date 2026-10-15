/*
 * ================================================================
 *  UnlimitedTetris — src/unit2_scan.cpp
 *
 *  Unit 2: Scan Conversion Algorithms
 *  ────────────────────────────────────
 *  2.1  Bresenham's Line Algorithm
 *       dx = |x1-x0|, dy = |y1-y0|, err = dx-dy
 *       Each step: 2*err > -dy → move x; 2*err < dx → move y
 *       Zero floating-point in the inner loop.
 *  2.3  Polygon fill via GL_QUADS (scan-line rasteriser)
 * ================================================================
 */

#include "../include/unit2_scan.h"

// ── 2.1  Bresenham integer line ───────────────────────────────
void bline(int x0, int y0, int x1, int y1,
           float r, float g, float b, float a)
{
    glColor4f(r,g,b,a);
    int dx=abs(x1-x0), dy=abs(y1-y0);
    int sx=(x0<x1)?1:-1, sy=(y0<y1)?1:-1;
    int err=dx-dy;
    glBegin(GL_POINTS);
    while(true){
        glVertex2i(x0,y0);
        if(x0==x1&&y0==y1) break;
        int e2=2*err;
        if(e2>-dy){err-=dy;x0+=sx;}
        if(e2< dx){err+=dx;y0+=sy;}
    }
    glEnd();
}

// ── 2.3  Filled quad (scan-line polygon fill) ─────────────────
void quad(float x0, float y0, float x1, float y1,
          float r,  float g,  float b,  float a)
{
    glColor4f(r,g,b,a);
    glBegin(GL_QUADS);
    glVertex2f(x0,y0); glVertex2f(x1,y0);
    glVertex2f(x1,y1); glVertex2f(x0,y1);
    glEnd();
}

// ── Board grid drawn with Bresenham lines ─────────────────────
void drawGrid()
{
    for(int c=0;c<=COLS;c++) bline(c*CELL,0,c*CELL,WIN_H,.17f,.17f,.21f);
    for(int r=0;r<=ROWS;r++) bline(0,r*CELL,COLS*CELL,r*CELL,.17f,.17f,.21f);
}

// ── Text helpers (Unit 10.1 — bitmap fonts) ───────────────────
void textTitle(float x, float y, const std::string& s,
               float r, float g, float b)
{
    glColor3f(r, g, b);
    glPushMatrix();

    // Stroke fonts are drawn in a ~100-unit em square.
    // A scale of 0.25 ≈ 25 units tall — adjust to taste.
    const float SCALE = 0.25f;

    glTranslatef(x, y, 0.0f);
    glScalef(SCALE, SCALE, 1.5f);

    for (char c : s) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
        // glutStrokeWidth advances the "pen" automatically
    }

    glPopMatrix();
}
void textHeading(float x, float y, const std::string& s,
                  float r, float g, float b)
{
    glColor3f(r,g,b);
    glRasterPos2f(x,y);
    for(char c : s) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
}

void text18(float x, float y, const std::string& s,
            float r, float g, float b)
{
    glColor3f(r,g,b); glRasterPos2f(x,y);
    for(char c:s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,c);
}

void textLabel(float x, float y, const std::string& s,
               float r, float g, float b)
{
    glColor3f(r,g,b); glRasterPos2f(x,y);
    for(char c:s) glutBitmapCharacter(GLUT_BITMAP_9_BY_15,c);
}

void text12(float x, float y, const std::string& s,
            float r, float g, float b)
{
    glColor3f(r,g,b); glRasterPos2f(x,y);
    for(char c:s) glutBitmapCharacter(GLUT_BITMAP_8_BY_13,c);
}
