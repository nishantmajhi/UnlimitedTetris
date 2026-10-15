#ifndef UNIT2_SCAN_H
#define UNIT2_SCAN_H

/*
 * ================================================================
 *  UnlimitedTetris — include/unit2_scan.h
 *
 *  Unit 2: Scan Conversion Algorithms
 *  ────────────────────────────────────
 *  2.1  Bresenham's Line Algorithm  → bline()
 *  2.3  Polygon fill (scan-line)    → quad()
 *       Grid, board cells, UI panels all use these primitives.
 * ================================================================
 */

#include "gfx.h"
#include "globals.h"

// 2.1  Bresenham integer line — no floating-point in inner loop
void bline(int x0, int y0, int x1, int y1,
           float r, float g, float b, float a = 1.0f);

// 2.3  Filled axis-aligned quad via GL_QUADS
void quad(float x0, float y0, float x1, float y1,
          float r,  float g,  float b,  float a = 1.0f);

// Board grid drawn entirely with Bresenham lines
void drawGrid();

// Text rendering helpers (bitmap fonts — Unit 10.1)
void textTitle (float x, float y, const std::string& s,
                float r = 1, float g = 1, float b = 1);
void textHeading (float x, float y, const std::string& s,
                float r = 1, float g = 1, float b = 1);
void text18    (float x, float y, const std::string& s,
                float r = 1, float g = 1, float b = 1);
void textLabel (float x, float y, const std::string& s,
                float r = 1, float g = 1, float b = 1);
void text12    (float x, float y, const std::string& s,
                float r = 1, float g = 1, float b = 1);

#endif // UNIT2_SCAN_H
