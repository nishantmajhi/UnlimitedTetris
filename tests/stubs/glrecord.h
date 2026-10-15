#ifndef UT_GLRECORD_H
#define UT_GLRECORD_H

/*
 * ================================================================
 *  UnlimitedTetris — tests/stubs/glrecord.h
 *
 *  What the fake OpenGL wrote down. The point of recording the
 *  drawing rather than discarding it is that the numbers on the
 *  screen become assertable: a test can run the real display()
 *  and read back the string the panel printed next to "BEST".
 * ================================================================
 */

#include <string>
#include <vector>

struct RecVertex { float x, y, z; };

struct RecPrim {
    unsigned int           mode;            // GL_QUADS, GL_LINES, ...
    float                  r, g, b, a;      // colour at glBegin time
    std::vector<RecVertex> verts;
};

struct RecText {
    float       x, y;      // raster position, or translate for stroke text
    int         font;      // 1=stroke roman, 2=times24, 3=helv18, 4=9x15, 5=8x13
    std::string s;
};

struct RecTimer { unsigned int msecs; void (*fn)(int); int value; };

void                         gl_reset();
void                         gl_flush();        // close any open text run
const std::vector<RecPrim>&  gl_prims();
const std::vector<RecText>&  gl_texts();
const std::vector<RecTimer>& gl_timers();
int                          gl_redisplays();
int                          gl_swaps();

// Every character drawn this frame, in draw order, runs separated
// by '\n'. Used for "did the screen say this anywhere" assertions.
std::string gl_allText();

// The text of the run that follows the run whose text equals
// `label` — i.e. the value printed under a heading such as "BEST".
// Returns "" if the label was never drawn or drew nothing after it.
std::string gl_textAfter(const std::string& label);

int gl_countPrims(unsigned int mode);

#endif // UT_GLRECORD_H
