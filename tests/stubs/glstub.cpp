/*
 * ================================================================
 *  UnlimitedTetris — tests/stubs/glstub.cpp
 *
 *  Implementation of the fake GL/GLU/GLUT declared in
 *  tests/stubs/GL/glut.h. It draws nothing; it writes down what
 *  it was asked to draw so the tests can assert on it.
 * ================================================================
 */

#include <GL/glut.h>
#include "glrecord.h"

// Font tokens — only their addresses matter.
void* glutStrokeRomanFont          = (void*)1;
void* glutBitmapTimesRoman24Font   = (void*)2;
void* glutBitmapHelvetica18Font    = (void*)3;
void* glutBitmap9By15Font          = (void*)4;
void* glutBitmap8By13Font          = (void*)5;

namespace {

std::vector<RecPrim>  g_prims;
std::vector<RecText>  g_texts;
std::vector<RecTimer> g_timers;

float g_r = 1, g_g = 1, g_b = 1, g_a = 1;
bool  g_inPrim = false;
RecPrim g_cur;

int   g_redisplays = 0;
int   g_swaps      = 0;

// Pending text run: characters accumulate until the anchor moves.
bool        g_haveText = false;
RecText     g_text;
float       g_rasterX = 0, g_rasterY = 0;
float       g_transX  = 0, g_transY  = 0;

int fontId(void* f)
{
    if (f == glutStrokeRomanFont)        return 1;
    if (f == glutBitmapTimesRoman24Font) return 2;
    if (f == glutBitmapHelvetica18Font)  return 3;
    if (f == glutBitmap9By15Font)        return 4;
    if (f == glutBitmap8By13Font)        return 5;
    return 0;
}

void flushText()
{
    if (g_haveText && !g_text.s.empty()) g_texts.push_back(g_text);
    g_haveText = false;
    g_text.s.clear();
}

void beginText(float x, float y, int font)
{
    if (g_haveText && (g_text.font != font || g_text.x != x || g_text.y != y))
        flushText();
    if (!g_haveText) {
        g_text = RecText{ x, y, font, std::string() };
        g_haveText = true;
    }
}

} // namespace

// ── Test-facing accessors ─────────────────────────────────────
void gl_reset()
{
    g_prims.clear(); g_texts.clear(); g_timers.clear();
    g_redisplays = 0; g_swaps = 0;
    g_inPrim = false; g_haveText = false; g_text.s.clear();
    g_r = g_g = g_b = g_a = 1;
}

void gl_flush() { flushText(); }

const std::vector<RecPrim>&  gl_prims()  { return g_prims; }
const std::vector<RecText>&  gl_texts()  { return g_texts; }
const std::vector<RecTimer>& gl_timers() { return g_timers; }
int gl_redisplays() { return g_redisplays; }
int gl_swaps()      { return g_swaps; }

std::string gl_allText()
{
    flushText();
    std::string out;
    for (size_t i = 0; i < g_texts.size(); ++i) {
        if (i) out += '\n';
        out += g_texts[i].s;
    }
    return out;
}

std::string gl_textAfter(const std::string& label)
{
    flushText();
    for (size_t i = 0; i + 1 < g_texts.size(); ++i)
        if (g_texts[i].s == label) return g_texts[i + 1].s;
    return std::string();
}

int gl_countPrims(unsigned int mode)
{
    int n = 0;
    for (size_t i = 0; i < g_prims.size(); ++i)
        if (g_prims[i].mode == mode) ++n;
    return n;
}

// ── Fake OpenGL ───────────────────────────────────────────────
void glBegin(GLenum mode)
{
    flushText();
    g_inPrim = true;
    g_cur = RecPrim();
    g_cur.mode = mode;
    g_cur.r = g_r; g_cur.g = g_g; g_cur.b = g_b; g_cur.a = g_a;
}

void glEnd()
{
    if (g_inPrim) g_prims.push_back(g_cur);
    g_inPrim = false;
}

void glVertex2i(GLint x, GLint y)
{ if (g_inPrim) g_cur.verts.push_back(RecVertex{ (float)x, (float)y, 0.0f }); }

void glVertex2f(GLfloat x, GLfloat y)
{ if (g_inPrim) g_cur.verts.push_back(RecVertex{ x, y, 0.0f }); }

void glVertex3f(GLfloat x, GLfloat y, GLfloat z)
{ if (g_inPrim) g_cur.verts.push_back(RecVertex{ x, y, z }); }

void glColor3f(GLfloat r, GLfloat g, GLfloat b)
{ g_r = r; g_g = g; g_b = b; g_a = 1.0f; }

void glColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{ g_r = r; g_g = g; g_b = b; g_a = a; }

void glLineWidth(GLfloat)  {}

void glRasterPos2f(GLfloat x, GLfloat y)
{ flushText(); g_rasterX = x; g_rasterY = y; }

void glEnable(GLenum)      {}
void glDisable(GLenum)     {}
void glBlendFunc(GLenum, GLenum) {}
void glClear(GLbitfield)   {}
void glClearColor(GLfloat, GLfloat, GLfloat, GLfloat) {}
void glViewport(GLint, GLint, GLint, GLint) {}

void glMatrixMode(GLenum)  {}
void glLoadIdentity()      {}
void glPushMatrix()        {}
void glPopMatrix()         { flushText(); }
void glTranslatef(GLfloat x, GLfloat y, GLfloat)
{ flushText(); g_transX = x; g_transY = y; }
void glScalef(GLfloat, GLfloat, GLfloat) {}
void glRotatef(GLfloat, GLfloat, GLfloat, GLfloat) {}
void glOrtho(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble) {}

void gluPerspective(GLdouble, GLdouble, GLdouble, GLdouble) {}
void gluLookAt(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble,
               GLdouble, GLdouble, GLdouble, GLdouble) {}

void glutInit(int*, char**)          {}
void glutInitDisplayMode(unsigned int) {}
void glutInitWindowSize(int, int)    {}
void glutInitWindowPosition(int, int) {}
int  glutCreateWindow(const char*)   { return 1; }
void glutDisplayFunc(void (*)())     {}
void glutReshapeFunc(void (*)(int, int)) {}
void glutKeyboardFunc(void (*)(unsigned char, int, int)) {}
void glutSpecialFunc(void (*)(int, int, int)) {}

void glutTimerFunc(unsigned int msecs, void (*f)(int), int value)
{ g_timers.push_back(RecTimer{ msecs, f, value }); }

void glutPostRedisplay()   { ++g_redisplays; }
void glutSwapBuffers()     { ++g_swaps; }
void glutReshapeWindow(int, int) {}
void glutMainLoop()        {}

void glutBitmapCharacter(void* font, int c)
{
    beginText(g_rasterX, g_rasterY, fontId(font));
    g_text.s += (char)c;
}

void glutStrokeCharacter(void* font, int c)
{
    beginText(g_transX, g_transY, fontId(font));
    g_text.s += (char)c;
}

int glutStrokeWidth(void*, int) { return 10; }
