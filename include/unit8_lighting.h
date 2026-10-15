#ifndef UNIT8_LIGHTING_H
#define UNIT8_LIGHTING_H

/*
 * ================================================================
 *  UnlimitedTetris — include/unit8_lighting.h
 *
 *  Unit 8: Illumination Models & Surface Rendering
 *  ─────────────────────────────────────────────────
 *  8.1  Phong model (per cell, in software):
 *         I = Ia·Ka + Id·Kd·(N·L) + Is·Ks·(R·V)^n
 *         Ka = 0.55  ambient fill
 *         Kd = 0.90  bright top-left bevel  (N·L ≈ 0.9)
 *         Ks = 0.40  specular dot (white GL_TRIANGLE_FAN)
 *  8.2  Intensity attenuation → dark bottom-right shadow bevel
 *  8.3  Constant shading  → ghost cells (ambient only)
 *       Phong shading     → active + locked cells
 * ================================================================
 */

#include "gfx.h"
#include "globals.h"

// 8.1/8.3  Full Phong cell: ambient + diffuse bevel + specular dot + shadow
void drawCell(int col, int row, float r, float g, float b, float a = 1.0f);

// 8.3  Ghost cell: constant ambient only, low opacity
void drawGhost(int col, int row, float r, float g, float b);

// Next-piece mini preview cell (panel sidebar)
void drawPreview(float cx, float cy, int pieceType);

// Clamp helper — shared by lighting calculations
float clamp1(float v);

#endif // UNIT8_LIGHTING_H
