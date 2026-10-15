#ifndef GFX_H
#define GFX_H

/*
 * ================================================================
 *  UnlimitedTetris — include/gfx.h
 *  The one place OpenGL enters the program.
 * ================================================================
 *  globals.h deliberately does NOT include this. The rules of the
 *  game — collision, line clearing, scoring, the menu state
 *  machine — compile and link with no graphics library present,
 *  which is what lets tests/run.cpp exercise them directly. Only
 *  the drawing units include this header.
 * ================================================================
 */

#ifdef _WIN32
  #include <windows.h>   // must precede GL on Windows: APIENTRY, WINGDIAPI
#endif
#include <GL/glut.h>

#endif // GFX_H
