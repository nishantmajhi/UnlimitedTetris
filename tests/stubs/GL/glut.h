#ifndef UT_FAKE_GLUT_H
#define UT_FAKE_GLUT_H

/*
 * ================================================================
 *  UnlimitedTetris — tests/stubs/GL/glut.h
 *
 *  A stand-in for the real <GL/glut.h>, used ONLY by the test
 *  build. Putting tests/stubs first on the include path makes the
 *  real source files compile unmodified against the recorder in
 *  glstub.cpp instead of against OpenGL and freeglut.
 *
 *  Why a fake header rather than fake functions: the genuine
 *  declarations are __declspec(dllimport) on Windows, so a local
 *  definition of glVertex2f is a compile error. Replacing the
 *  header removes the dllimport and nothing else.
 *
 *  Everything declared here is something the project actually
 *  calls. The list comes from grepping the sources for every
 *  identifier beginning gl, glu or glut; if a new call appears,
 *  the test build fails and this file is where it is added.
 * ================================================================
 */

typedef float        GLfloat;
typedef double       GLdouble;
typedef int          GLint;
typedef unsigned int GLenum;
typedef unsigned int GLbitfield;
typedef void         GLvoid;

// ── Primitive modes and enums ─────────────────────────────────
#define GL_POINTS                 0x0000
#define GL_LINES                  0x0001
#define GL_LINE_LOOP              0x0002
#define GL_LINE_STRIP             0x0003
#define GL_TRIANGLES              0x0004
#define GL_TRIANGLE_FAN           0x0006
#define GL_QUADS                  0x0007

#define GL_DEPTH_TEST             0x0B71
#define GL_BLEND                  0x0BE2
#define GL_SRC_ALPHA              0x0302
#define GL_ONE_MINUS_SRC_ALPHA    0x0303
#define GL_MODELVIEW              0x1700
#define GL_PROJECTION             0x1701
#define GL_COLOR_BUFFER_BIT       0x00004000
#define GL_DEPTH_BUFFER_BIT       0x00000100

// ── GLUT display mode / special keys ──────────────────────────
#define GLUT_RGBA                 0x0000
#define GLUT_DOUBLE               0x0002
#define GLUT_DEPTH                0x0010

#define GLUT_KEY_LEFT             100
#define GLUT_KEY_UP               101
#define GLUT_KEY_RIGHT            102
#define GLUT_KEY_DOWN             103

// ── Font handles ──────────────────────────────────────────────
//  The real ones are opaque void* tokens; these only have to be
//  distinct, because the recorder stores which font was asked for.
extern void* glutStrokeRomanFont;
extern void* glutBitmapTimesRoman24Font;
extern void* glutBitmapHelvetica18Font;
extern void* glutBitmap9By15Font;
extern void* glutBitmap8By13Font;

#define GLUT_STROKE_ROMAN            glutStrokeRomanFont
#define GLUT_BITMAP_TIMES_ROMAN_24   glutBitmapTimesRoman24Font
#define GLUT_BITMAP_HELVETICA_18     glutBitmapHelvetica18Font
#define GLUT_BITMAP_9_BY_15          glutBitmap9By15Font
#define GLUT_BITMAP_8_BY_13          glutBitmap8By13Font

// ── Immediate-mode drawing ────────────────────────────────────
void glBegin(GLenum mode);
void glEnd();
void glVertex2i(GLint x, GLint y);
void glVertex2f(GLfloat x, GLfloat y);
void glVertex3f(GLfloat x, GLfloat y, GLfloat z);
void glColor3f(GLfloat r, GLfloat g, GLfloat b);
void glColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
void glLineWidth(GLfloat w);
void glRasterPos2f(GLfloat x, GLfloat y);

// ── State ─────────────────────────────────────────────────────
void glEnable(GLenum cap);
void glDisable(GLenum cap);
void glBlendFunc(GLenum src, GLenum dst);
void glClear(GLbitfield mask);
void glClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
void glViewport(GLint x, GLint y, GLint w, GLint h);

// ── Matrix stack ──────────────────────────────────────────────
void glMatrixMode(GLenum mode);
void glLoadIdentity();
void glPushMatrix();
void glPopMatrix();
void glTranslatef(GLfloat x, GLfloat y, GLfloat z);
void glScalef(GLfloat x, GLfloat y, GLfloat z);
void glRotatef(GLfloat a, GLfloat x, GLfloat y, GLfloat z);
void glOrtho(GLdouble l, GLdouble r, GLdouble b, GLdouble t,
             GLdouble n, GLdouble f);

// ── GLU ───────────────────────────────────────────────────────
void gluPerspective(GLdouble fovy, GLdouble aspect,
                    GLdouble zNear, GLdouble zFar);
void gluLookAt(GLdouble ex, GLdouble ey, GLdouble ez,
               GLdouble cx, GLdouble cy, GLdouble cz,
               GLdouble ux, GLdouble uy, GLdouble uz);

// ── GLUT ──────────────────────────────────────────────────────
void glutInit(int* argc, char** argv);
void glutInitDisplayMode(unsigned int mode);
void glutInitWindowSize(int w, int h);
void glutInitWindowPosition(int x, int y);
int  glutCreateWindow(const char* title);
void glutDisplayFunc(void (*f)());
void glutReshapeFunc(void (*f)(int, int));
void glutKeyboardFunc(void (*f)(unsigned char, int, int));
void glutSpecialFunc(void (*f)(int, int, int));
void glutTimerFunc(unsigned int msecs, void (*f)(int), int value);
void glutPostRedisplay();
void glutSwapBuffers();
void glutReshapeWindow(int w, int h);
void glutMainLoop();
void glutBitmapCharacter(void* font, int character);
void glutStrokeCharacter(void* font, int character);
int  glutStrokeWidth(void* font, int character);

#endif // UT_FAKE_GLUT_H
